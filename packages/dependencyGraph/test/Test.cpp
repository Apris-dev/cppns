
#include <iostream>
#include <string>
#include <chrono>
#include <memory>

#include "cppns/dg/DependencyGraph.h"
#include "cppns/memory/Memory.h"
#include "cppns/container/Map.h"
#include "cppns/container/Vector.h"

using namespace std::chrono;

struct SObject {

    SObject(const std::string& name): name(name) {}

    void print() const {
        std::cout << name << std::endl;
    }

    std::string name;
};

struct SResource {
    size_t id = 0;

    explicit SResource(const size_t inId): id(inId) {}

    friend COutputArchive& operator<<(COutputArchive& archive, const SResource& resource) {
        archive << resource.id;
        return archive;
    }
};

struct SBuffer : SResource {
    using SResource::SResource;

    SBuffer(const std::string_view inName, const size_t id): SResource(id), name(inName) {}

    std::string name;
};

struct TestCommandBuffer {

    struct PassInfo {
        std::string name;
        size_t nodeId; // ID in the dependency graph
    };

    struct ResourceLifetime {
        TShared<SBuffer> buf;
        bool externallyOwned = false; // skip automatic destruction
        size_t firstPassIndex = SIZE_MAX; // pass where resource is first used
        size_t lastPassIndex  = 0;        // pass where resource is last used
    };

    void markExternal(const TShared<SBuffer>& buffer) {
        if (!lifetimes.isValid(buffer->id)) {
            lifetimes.push(buffer->id, ResourceLifetime{buffer, true});
            return;
        }
        lifetimes.get(buffer->id).externallyOwned = true;
    }

    void trackResource(const TShared<SBuffer>& buffer) {
        if (!lifetimes.isValid(buffer->id)) {
            lifetimes.push(buffer->id, ResourceLifetime{buffer});
        }
        auto& [buf, ext, firstPassIndex, lastPassIndex] = lifetimes.get(buffer->id);
        if (currentPassIndex < firstPassIndex) firstPassIndex = currentPassIndex;
        if (currentPassIndex > firstPassIndex) lastPassIndex = currentPassIndex;
    }

    void startNewPass(const std::string& string) {
        currentPassIndex = dependencyGraph.addNode(TShared<SObject>(string));
        passes.push({string, currentPassIndex});
    }

    // Read
    void bindVertexBuffers(const TShared<SBuffer>& buffer) {
        trackResource(buffer);
        dependencyGraph.addRead(currentPassIndex, buffer.staticCast<SResource>());
    }

    // Read
    void use(const TShared<SBuffer>& buffer) {
        trackResource(buffer);
        dependencyGraph.addRead(currentPassIndex, buffer.staticCast<SResource>());
    }

    // Write
    void draw(const TShared<SBuffer>& buffer) {
        trackResource(buffer);
        dependencyGraph.addWrite(currentPassIndex, buffer.staticCast<SResource>());
    }

    // Read from one buffer, write to another
    void copyBuffer(const TShared<SBuffer>& src, const TShared<SBuffer>& dst) {
        trackResource(src);
        trackResource(dst);
        dependencyGraph.addRead(currentPassIndex, src.staticCast<SResource>());
        dependencyGraph.addWrite(currentPassIndex, dst.staticCast<SResource>());
    }

    // After recording, infer destruction order from lifetimes
    TVector<TShared<SBuffer>> buildDestructionOrder() {
        // Sort resources by their last used pass, descending
        // Resources last used latest are destroyed first? No —
        // resources last used EARLIEST are destroyed first (they're done sooner)
        TVector<TPair<size_t, ResourceLifetime>> sorted;
        for (auto& lifetime : lifetimes) {
            if (lifetime.second().externallyOwned) continue;
            sorted.push(lifetime);
        }

        sorted.sort([](const auto& fst, const auto& snd) {
            return fst.second().lastPassIndex < snd.second().lastPassIndex; // earliest last-use first
        });

        TVector<TShared<SBuffer>> destructionOrder;
        for (auto& pair : sorted)
            destructionOrder.push(pair.second().buf);

        return destructionOrder;
    }

    TVector<PassInfo> passes;

    TMap<size_t, ResourceLifetime> lifetimes;

    size_t currentPassIndex = 0;

    mutable TRWDependencyGraph<TShared<SObject>, TShared<SResource>, TKahnTopologicalSort> dependencyGraph;
};

EXPORTC int run() {

    {
        TestCommandBuffer cmd{};

        cmd.startNewPass("Base Pass");

        // Enforced Unique IDs per node
        TShared<SBuffer> vBuffer{"vBuffer", 0};
        TShared<SBuffer> colorBuffer{"colorBuffer", 1};
        TShared<SBuffer> depthBuffer{"depthBuffer", 2};
        TShared<SBuffer> finalBuffer{"finalBuffer", 3};

        // Binding vertices
        cmd.bindVertexBuffers(vBuffer);

        // Drawing on gBuffer
        cmd.draw(colorBuffer);
        cmd.draw(depthBuffer);

        // Copy to end
        cmd.copyBuffer(colorBuffer, finalBuffer);

        cmd.startNewPass("Reflection Pass");

        TShared<SBuffer> postReflect{"postReflect", 4};

        cmd.use(finalBuffer);
        cmd.use(depthBuffer);
        cmd.draw(postReflect);

        cmd.startNewPass("Post Process");

        TShared<SBuffer> ppBuffer{"ppBuffer", 5};
        cmd.markExternal(ppBuffer);

        cmd.use(postReflect);

        // Post process (a few effects or so)
        cmd.draw(finalBuffer);
        cmd.draw(finalBuffer);
        cmd.draw(finalBuffer);

        cmd.copyBuffer(finalBuffer, ppBuffer);

        const auto order = cmd.buildDestructionOrder();

        for (const auto& resource : order)
            std::cout << "Destroy resource " << resource->name << " after pass "
                      << cmd.passes[cmd.lifetimes.get(resource->id).lastPassIndex].name << "\n";

        // We should be able to infer what resource we can destroy first, and what can be done in parallel (have multiple threads destruct or so)
        // A good way to do this is to construct another graph

        return 0;
    }

    {
        TRWDependencyGraph<TShared<SObject>, TShared<SResource>, TKahnTopologicalSort> graph;

        const TShared<SResource> hdrColor{0};
        const TShared<SResource> depth{1};
        const TShared<SResource> history{2};

        // GBuffer writes HDR + depth
        size_t gbufferPass = graph.addNode(TShared<SObject>("gbufferPass"));
        graph.addWrite(gbufferPass, hdrColor);
        graph.addWrite(gbufferPass, depth);

        // Lighting reads GBuffer HDR + depth, writes HDR (in-place lighting)
        size_t lightingPass = graph.addNode(TShared<SObject>("lightingPass"));
        graph.addRead(lightingPass, hdrColor);
        graph.addRead(lightingPass, depth);
        graph.addWrite(lightingPass, hdrColor);

        // TAA reads current HDR + history, writes HDR
        size_t taaPass = graph.addNode(TShared<SObject>("taaPass"));
        graph.addRead(taaPass, hdrColor);
        graph.addRead(taaPass, history);
        graph.addWrite(taaPass, hdrColor);

        // Bloom threshold reads HDR, writes HDR (destructive)
        size_t bloomThresholdPass = graph.addNode(TShared<SObject>("bloomThresholdPass"));
        graph.addRead(bloomThresholdPass, hdrColor);
        graph.addWrite(bloomThresholdPass, hdrColor);

        // Upscale reads HDR, writes HDR
        size_t upscalePass = graph.addNode(TShared<SObject>("upscalePass"));
        graph.addRead(upscalePass, hdrColor);
        graph.addWrite(upscalePass, hdrColor);

        // Post-process reads HDR, writes HDR
        size_t postProcessPass = graph.addNode(TShared<SObject>("postProcessPass"));
        graph.addRead(postProcessPass, hdrColor);
        graph.addWrite(postProcessPass, hdrColor);

        // History resolve reads final HDR, writes history (feedback loop)
        size_t historyResolvePass = graph.addNode(TShared<SObject>("historyResolvePass"));
        graph.addRead(historyResolvePass, hdrColor);
        graph.addWrite(historyResolvePass, history);

#ifdef USING_CPPNS_CONTAINER
        const auto order = graph.buildExecutionOrder();

        for (const auto& node : order) {
            std::cout << graph.getNode(node)->name << " -> ";
        }
#else
        const auto order = graph.buildExecutionOrder();

        for (const auto& node : order) {
            std::cout << graph.getNode(node)->name << " -> ";
        }
#endif
        std::cout << std::endl << std::endl;

        /*
        Resource lifetime tracking,
        aliasing,
        barrier synthesis,
        pass culling,
        track resource usage,
        unused write culling,
        emit barriers,
        and split graphics
         */
    }

    return 0;
}