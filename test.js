var gcHeapWatcher = require('./')();
gcHeapWatcher.on('stats', function (stats) {
    console.log(JSON.stringify(stats, null, 2));
});

setInterval(function () {
    var big = new Array(1024 * 74);
}, 1000)

setTimeout(function () {
    console.log('unbind!')
    gcHeapWatcher.stop();
}, 5000)