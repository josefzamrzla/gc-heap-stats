#include <node.h>
#include <vector>
#include <cmath>
#include <uv.h>

using namespace v8;

namespace gc_heap_stats {
    
    Persistent<Function> callback;
    
    struct HeapStats {
        GCType type;
        #if NODE_MAJOR_VERSION >= 3
        std::vector<HeapSpaceStatistics> spaces;
        #endif
        HeapStatistics heapStats;
        uint64_t time;
        HeapStats(Isolate *isolate, GCType gcType) {
            type = gcType;
            time = uv_hrtime();
            #if NODE_MAJOR_VERSION >= 3
            HeapSpaceStatistics stats;
            for (size_t i = 0; i < isolate->NumberOfHeapSpaces(); ++i) {
                isolate->GetHeapSpaceStatistics(&stats, i);
                spaces.push_back(stats);
            }
            #endif
            isolate->GetHeapStatistics(&heapStats);
        }
    };
    
    HeapStats * beforeGCStats;
    
    Local<Object> packStatsStep(Isolate *isolate, HeapStats *step) {
        Local<Object> stats = Object::New(isolate);
        stats->Set(String::NewFromUtf8(isolate, "total_heap_size"), Number::New(isolate, step->heapStats.total_heap_size()));
        stats->Set(String::NewFromUtf8(isolate, "total_heap_size_executable"), Number::New(isolate, step->heapStats.total_heap_size_executable()));
        stats->Set(String::NewFromUtf8(isolate, "total_physical_size"), Number::New(isolate, step->heapStats.total_physical_size()));

        #if NODE_MAJOR_VERSION >= 3
        stats->Set(String::NewFromUtf8(isolate, "total_available_size"), Number::New(isolate, step->heapStats.total_available_size()));
        #endif

        stats->Set(String::NewFromUtf8(isolate, "used_heap_size"), Number::New(isolate, step->heapStats.used_heap_size()));
        stats->Set(String::NewFromUtf8(isolate, "heap_size_limit"), Number::New(isolate, step->heapStats.heap_size_limit()));

        #if NODE_MAJOR_VERSION >= 6
        stats->Set(String::NewFromUtf8(isolate, "does_zap_garbage"), Number::New(isolate, step->heapStats.does_zap_garbage()));
        #endif

        #if NODE_MAJOR_VERSION >= 3
        Local<Array> spaces = Array::New(isolate);
        
        for (size_t i = 0; i < step->spaces.size(); ++i) {
            Local<Object> space = Object::New(isolate);
            space->Set(String::NewFromUtf8(isolate, "space_name"), String::NewFromUtf8(isolate, step->spaces[i].space_name()));
            space->Set(String::NewFromUtf8(isolate, "space_size"), Number::New(isolate, step->spaces[i].space_size()));
            space->Set(String::NewFromUtf8(isolate, "space_used_size"), Number::New(isolate, step->spaces[i].space_used_size()));
            space->Set(String::NewFromUtf8(isolate, "space_available_size"), Number::New(isolate, step->spaces[i].space_available_size()));
            space->Set(String::NewFromUtf8(isolate, "physical_space_size"), Number::New(isolate, step->spaces[i].physical_space_size()));

            spaces->Set(i, space);
        }
        
        stats->Set(String::NewFromUtf8(isolate, "spaces"), spaces);
        #endif

        return stats;
    }

    Local<Object> packStatsDiff(Isolate *isolate, HeapStats *before, HeapStats *after) {
        Local<Object> diff = Object::New(isolate);
        diff->Set(
            String::NewFromUtf8(isolate, "total_heap_size"), 
            Number::New(isolate, static_cast<double>(after->heapStats.total_heap_size()) - static_cast<double>(before->heapStats.total_heap_size())));
        diff->Set(
            String::NewFromUtf8(isolate, "total_heap_size_executable"), 
            Number::New(isolate, static_cast<double>(after->heapStats.total_heap_size_executable()) - static_cast<double>(before->heapStats.total_heap_size_executable())));
        diff->Set(
            String::NewFromUtf8(isolate, "total_physical_size"), 
            Number::New(isolate, static_cast<double>(after->heapStats.total_physical_size()) - static_cast<double>(before->heapStats.total_physical_size())));

        #if NODE_MAJOR_VERSION >= 3
        diff->Set(
            String::NewFromUtf8(isolate, "total_available_size"), 
            Number::New(isolate, static_cast<double>(after->heapStats.total_available_size()) - static_cast<double>(before->heapStats.total_available_size())));
        #endif

        diff->Set(
            String::NewFromUtf8(isolate, "used_heap_size"), 
            Number::New(isolate, static_cast<double>(after->heapStats.used_heap_size()) - static_cast<double>(before->heapStats.used_heap_size())));
        diff->Set(
            String::NewFromUtf8(isolate, "heap_size_limit"), 
            Number::New(isolate, static_cast<double>(after->heapStats.heap_size_limit()) - static_cast<double>(before->heapStats.heap_size_limit())));

        #if NODE_MAJOR_VERSION >= 6
        diff->Set(
            String::NewFromUtf8(isolate, "does_zap_garbage"), 
            Number::New(isolate, static_cast<double>(after->heapStats.does_zap_garbage()) - static_cast<double>(before->heapStats.does_zap_garbage())));
        #endif

        #if NODE_MAJOR_VERSION >= 3
        Local<Array> spaces = Array::New(isolate);
        
        for (size_t i = 0; i < before->spaces.size(); ++i) {
            Local<Object> space = Object::New(isolate);
            space->Set(String::NewFromUtf8(isolate, "space_name"), String::NewFromUtf8(isolate, before->spaces[i].space_name()));
            space->Set(
                String::NewFromUtf8(isolate, "space_size"), 
                Number::New(isolate, static_cast<double>(after->spaces[i].space_size()) - static_cast<double>(before->spaces[i].space_size())));
            space->Set(
                String::NewFromUtf8(isolate, "space_used_size"), 
                Number::New(isolate, static_cast<double>(after->spaces[i].space_used_size()) - static_cast<double>(before->spaces[i].space_used_size())));
            space->Set(
                String::NewFromUtf8(isolate, "space_available_size"), 
                Number::New(isolate, static_cast<double>(after->spaces[i].space_available_size()) - static_cast<double>(before->spaces[i].space_available_size())));
            space->Set(
                String::NewFromUtf8(isolate, "physical_space_size"), 
                Number::New(isolate, static_cast<double>(after->spaces[i].physical_space_size()) - static_cast<double>(before->spaces[i].physical_space_size())));

            spaces->Set(i, space);
        }
        
        diff->Set(String::NewFromUtf8(isolate, "spaces"), spaces);
        #endif

        return diff;
    }
    
    Local<Object> packStats(Isolate *isolate, HeapStats *before, HeapStats *after) {
        Local<Object> stats = Object::New(isolate);
        stats->Set(String::NewFromUtf8(isolate, "type"), Number::New(isolate, before->type));
        stats->Set(String::NewFromUtf8(isolate, "took"), Number::New(isolate, round((after->time - before->time) / 1000.0) / 1000.0));
        
        stats->Set(String::NewFromUtf8(isolate, "before"), packStatsStep(isolate, before));
        stats->Set(String::NewFromUtf8(isolate, "after"), packStatsStep(isolate, after));
        stats->Set(String::NewFromUtf8(isolate, "diff"), packStatsDiff(isolate, before, after));
        return stats;
    }
    
    void preGcCallback(Isolate *isolate, GCType type, GCCallbackFlags flags) {
        beforeGCStats = new HeapStats(isolate, type);
    }

    void postGcCallback(Isolate *isolate, GCType type, GCCallbackFlags flags) {
        HeapStats * afterGCStats = new HeapStats(isolate, type);

        Local<Function> cb = Local<Function>::New(isolate, callback);
        Local<Value> argv[1] = {packStats(isolate, beforeGCStats, afterGCStats)};
        cb->Call(Null(isolate), 1, argv);
        
        delete beforeGCStats;
        delete afterGCStats;
    }
    
    void start(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if (!(args.Length() == 1 && args[0]->IsFunction())) {
            isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Missing callback argument"))); 
            return;
        }

        isolate->AddGCPrologueCallback(preGcCallback);
        isolate->AddGCEpilogueCallback(postGcCallback);

        callback.Reset(isolate, Local<Function>::Cast(args[0]));

        args.GetReturnValue().Set(Undefined(isolate));
    }

    void stop(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        
        isolate->RemoveGCPrologueCallback(preGcCallback);
        isolate->RemoveGCEpilogueCallback(postGcCallback);
        callback.Reset();
    }

    void init(Local<Object> exports) {
        NODE_SET_METHOD(exports, "start", start);
        NODE_SET_METHOD(exports, "stop", stop);
    }

    NODE_MODULE(gc_heap_stats, init)
}