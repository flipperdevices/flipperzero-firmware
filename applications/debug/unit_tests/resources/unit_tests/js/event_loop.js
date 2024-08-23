/// <reference types="../../../../../system/js_app/types/tests" />
let tests = require("tests");
/// <reference types="../../../../../system/js_app/types/event_loop" />
let event_loop = require("event_loop");

let ext_counter = { i: 0 };

event_loop.subscribe(event_loop.timer("periodic", 1), function (_, counter, ext_counter) {
    ext_counter.i++;
    if (counter === 10)
        event_loop.stop();
    return [counter + 1, ext_counter];
}, 1, ext_counter);

event_loop.subscribe(event_loop.timer("oneshot", 1000), function (_, tests) {
    tests.fail("event loop was not stopped");
}, tests);

event_loop.run();
tests.assert_eq(10, ext_counter.i);
