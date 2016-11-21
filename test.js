var assert = require('assert'),
    semver = require('semver');

describe('gc-heap-stats', function() {

    it('should emit stats event with object containing required list of properties', function(done) {

        var gcHeapStats = require('./')();
        var sectionProperties = [
            'total_heap_size',
            'total_heap_size_executable',
            'total_physical_size',
            'used_heap_size',
            'heap_size_limit'
        ];

        if(semver.gte(semver.clean(process.version), '4.0.0')) {
            sectionProperties.push('total_available_size');
        }

        gcHeapStats.on('stats', function (stats) {
            assert.ok(!!stats, 'Stats object is empty');
            assert.ok(
                'type' in stats && 'took' in stats && 'before' in stats && 'after' in stats && 'diff' in stats,
                'Stats object does not contain all required properties');

            ['before', 'after', 'diff'].forEach(function (section) {
                sectionProperties.forEach(function (prop) {
                    assert.ok(prop in stats[section], '\'' + section + '.' + prop + '\' property not found');
                });

                if(semver.gte(semver.clean(process.version), '4.0.0')) {
                    assert.ok(
                        Array.isArray(stats[section]['spaces']) && stats[section]['spaces'].length > 0,
                        'Spaces array not found or empty in \'' + section + '\' section');

                    stats[section]['spaces'].forEach(function (space) {
                        assert.ok(!!space, 'Space stats object is empty');
                        assert.ok(
                            'space_name' in space &&
                            'space_size' in space &&
                            'space_used_size' in space &&
                            'space_available_size' in space &&
                            'physical_space_size' in space,
                            'Space object does not contain all required properties'
                        );
                    });
                }
            });
            done();

        });

        // force GC run
        global.gc();
    });
});