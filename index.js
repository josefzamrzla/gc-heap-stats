var util = require('util'),
	events = require('events'),
	gcHeapStats = require('./build/Release/gc_heap_stats.node'),
	emitter;

var GcHeapStatsEmitter = function () {
	this.shouldStop = false;
};

util.inherits(GcHeapStatsEmitter, events.EventEmitter);

GcHeapStatsEmitter.prototype.stop = function () {
	this.shouldStop = true;
};

module.exports = function () {
	if (!emitter) {
		emitter = new GcHeapStatsEmitter();
		gcHeapStats.start(function (stats) {
			if (emitter.shouldStop) {
				gcHeapStats.stop();
				emitter.shouldStop = false;
				return;
			}

			emitter.emit('stats', stats);
		});
	}

	return emitter;
};