#cps_ppx

Hi guys, for me asynchronous computations are a matter of urgency, then I made a PPX that behind scene transform discret sequential code into AST continuation, for simple code style as an act to prove the viability and get helped to implement all the AST patterns.

if more people get on board, it can move further.

```
What works fully async by now:
------>    nested [@cps] function declaration
------>    function call
------>    single let bindings
------>    if/then/else
------>    match/cases
------>    try/cases

What will come fully async on short:
------>    for/while loops
------>    instance methods on cps apply detection
------>    ...
```