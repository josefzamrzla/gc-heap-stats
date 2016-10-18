# gc-heap-stats

Emits stats about V8 heap usage after each GC run.

## Install
```bash
npm install gc-heap-stats
```

## Basic usage
```javascript
var gcHeapStats = require('gc-heap-stats')();
gcHeapStats.on('stats', function (stats) {
    console.log(JSON.stringify(stats, null, 2));
});
```

## Advanced usage
```javascript
// init GC heap stats emitter as "deferred"
var gcHeapStats = require('gc-heap-stats')({deferred: true});
gcHeapStats.on('stats', function (stats) {
    console.log(JSON.stringify(stats, null, 2));
});

// no events will be emitted until you call
gcHeapStats.start();

// stop it
gcHeapStats.stop();

// ... start again, ... etc
gcHeapStats.start();
```

## Sample output

Sample output in node.js >= 4.0 (there won't be "spaces" array in older versions of node.js).
 
The output contains 3 sections: heap stats before GC was triggered, heap stats after GC run and the diff.

Top level properties:

* type: type of GC (can have one of the following values):
 * 1: Scavenge (minor GC)
 * 2: Mark/Sweep/Compact (major GC)
 * 4: Incremental marking
 * 8: Weak/Phantom callback processing
 * 15: All
* took: GC pause in milliseconds

Section properties:
* total_heap_size: Number of bytes V8 has allocated for the heap. This can grow if usedHeap needs more.
* used_heap_size: Number of bytes in used by application data
* total_heap_size_executable: Number of bytes for compiled bytecode and JITed code
* heap_size_limit: The absolute limit the heap cannot exceed (--max_old_space_size)
* total_physical_size: Committed size
 
Heap space properties:
* space_name: Name of a heap space
* space_size: Number of bytes allocated for a heap space 
* space_used_size: Number of bytes allocated (used) in a heap space 
* space_available_size: Number of bytes available in a heap space
* physical_space_size: Committed size of a heap space
 
```javascript
{
  "type": 1,
  "took": 1.102,
  "before": {
    "total_heap_size": 10522624,
    "total_heap_size_executable": 5242880,
    "total_physical_size": 10522624,
    "total_available_size": 1487252192,
    "used_heap_size": 4941096,
    "heap_size_limit": 1501560832,
    "spaces": [
      {
        "space_name": "new_space",
        "space_size": 4194304,
        "space_used_size": 1638368,
        "space_available_size": 424992,
        "physical_space_size": 4194304
      },
      {
        "space_name": "old_space",
        "space_size": 3100672,
        "space_used_size": 2499536,
        "space_available_size": 192,
        "physical_space_size": 3100672
      },
      {
        "space_name": "code_space",
        "space_size": 2097152,
        "space_used_size": 631328,
        "space_available_size": 0,
        "physical_space_size": 2097152
      },
      {
        "space_name": "map_space",
        "space_size": 1130496,
        "space_used_size": 171864,
        "space_available_size": 0,
        "physical_space_size": 1130496
      },
      {
        "space_name": "large_object_space",
        "space_size": 0,
        "space_used_size": 0,
        "space_available_size": 1486827008,
        "physical_space_size": 0
      }
    ]
  },
  "after": {
    "total_heap_size": 11571200,
    "total_heap_size_executable": 5242880,
    "total_physical_size": 11571200,
    "total_available_size": 1487833096,
    "used_heap_size": 3879584,
    "heap_size_limit": 1501560832,
    "spaces": [
      {
        "space_name": "new_space",
        "space_size": 4194304,
        "space_used_size": 8832,
        "space_available_size": 2054528,
        "physical_space_size": 4194304
      },
      {
        "space_name": "old_space",
        "space_size": 4149248,
        "space_used_size": 3067560,
        "space_available_size": 136,
        "physical_space_size": 4149248
      },
      {
        "space_name": "code_space",
        "space_size": 2097152,
        "space_used_size": 631328,
        "space_available_size": 0,
        "physical_space_size": 2097152
      },
      {
        "space_name": "map_space",
        "space_size": 1130496,
        "space_used_size": 171864,
        "space_available_size": 0,
        "physical_space_size": 1130496
      },
      {
        "space_name": "large_object_space",
        "space_size": 0,
        "space_used_size": 0,
        "space_available_size": 1485778432,
        "physical_space_size": 0
      }
    ]
  },
  "diff": {
    "total_heap_size": 1048576,
    "total_heap_size_executable": 0,
    "total_physical_size": 1048576,
    "total_available_size": 580904,
    "used_heap_size": -1061512,
    "heap_size_limit": 0,
    "spaces": [
      {
        "space_name": "new_space",
        "space_size": 0,
        "space_used_size": -1629536,
        "space_available_size": 1629536,
        "physical_space_size": 0
      },
      {
        "space_name": "old_space",
        "space_size": 1048576,
        "space_used_size": 568024,
        "space_available_size": -56,
        "physical_space_size": 1048576
      },
      {
        "space_name": "code_space",
        "space_size": 0,
        "space_used_size": 0,
        "space_available_size": 0,
        "physical_space_size": 0
      },
      {
        "space_name": "map_space",
        "space_size": 0,
        "space_used_size": 0,
        "space_available_size": 0,
        "physical_space_size": 0
      },
      {
        "space_name": "large_object_space",
        "space_size": 0,
        "space_used_size": 0,
        "space_available_size": -1048576,
        "physical_space_size": 0
      }
    ]
  }
}
```

## Compatibility

gc-heap-stats is compatible with node.js >= 0.12 however the best results (including V8 heap spaces stats) you will get with node.js >= 4.0. 
Version 0.10 is not supported, for node.js 0.10 please use similar package: https://github.com/dainis/node-gcstats