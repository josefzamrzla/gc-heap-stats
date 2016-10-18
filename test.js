var gcHeapStats = require('./')({deferred: true});
gcHeapStats.on('stats', function (stats) {
    console.log(JSON.stringify(stats, null, 2));
});

setInterval(function () {
    // force GC to run more frequently
    var big = new Array(1024 * 74);
}, 1000);

setTimeout(function () {
    console.log('### STOP!');
    gcHeapStats.stop();
}, 5000);

setTimeout(function () {
    console.log('### START AGAIN!');
    gcHeapStats.start();
}, 10000);

setTimeout(function () {
    console.log('### STOP AGAIN!');
    gcHeapStats.stop();
}, 15000);
