var util = require('util'),
	events = require('events'),
	gcHeapStats = require('bindings')('gc_heap_stats.node'),
	emitter = null;

var GcHeapStatsEmitter = function () {
	this.shouldStop = false;
	this.running = false;
};

util.inherits(GcHeapStatsEmitter, events.EventEmitter);

GcHeapStatsEmitter.prototype.stop = function () {
	if (!this.running) return;
	this.shouldStop = true;
};

GcHeapStatsEmitter.prototype.start = function () {
	if (this.running) return;
	var self = this;
	self.running = true;
	gcHeapStats.start(function (stats) {
		if (self.shouldStop) {
			gcHeapStats.stop();
			self.shouldStop = false;
			self.running = false;
			return;
		}

		self.emit('stats', stats);
	});
};

module.exports = function (opts) {
	opts = opts || {};

	if (!emitter) {
		emitter = new GcHeapStatsEmitter();
		if (!opts.deferred) {
			emitter.start();
		}
	}

	return emitter;
};