# Check libuv timer

Libuv repeating timer with a fixed timeout triggers callbacks not exactly with timeout between them.
There are additional delay is accumulating between callacks calls. 
This project is a try to make timer with accurate triggers without delay accumulation.