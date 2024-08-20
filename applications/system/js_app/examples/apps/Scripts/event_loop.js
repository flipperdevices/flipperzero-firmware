/// <reference types="../../../types/global" />
/// <reference types="../../../types/event_loop" />
let event_loop = require("event_loop");

event_loop.subscribe(event_loop.timer("periodic", 100), function (_subscription, event_loop, counter) {
    print("Counter:", counter);
    if (counter === 15)
        event_loop.stop();
    return [event_loop, counter + 1];
}, event_loop, 0);

event_loop.subscribe(event_loop.timer("oneshot", 1337), function (_subscription) {
    print("Hi after 1337 ms");
});

event_loop.run();
