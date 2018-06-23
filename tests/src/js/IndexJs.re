let println = print_endline;
let debugln = value => ();

exception Exception;
exception LocalException;

let firstErrorCps =
  [@cps]
  (
    x =>
      [@defer]
      {
        ();
        println("defer firstErrorCps");
        Js.Global.setTimeout(
          () => {
            println(">> continuation#error(LocalException)");
            continuation#error(LocalException);
          },
          1000,
        );
        /* raise(LocalException); */
      }
  );

let firstCps =
  [@cps]
  (
    x =>
      [@defer]
      {
        ();
        println("defer firstCps");
        /* continuation#resume(x ++ "-004");   */
        Js.Global.setTimeout(
          () => {
            println(">> continuation#resume(x ++ \"-004\")");
            continuation#resume(x ++ "-004");
          },
          1000,
        );
        
        /* continuation#error(Exception); */
      }
  );

/* let boolCps =
   [@cps]
   (
     x =>
       [@defer]
       {
         ();
         println("defer branch 10");
         continuation#resume(x);
       }
   ); */

let otherCps =
  [@cps]
  (
    x =>
      try (
        try (firstErrorCps(", 1")) {
        | LocalException =>
          println("a catch branch 1");
          raise(LocalException);
          firstCps(", 55");
        | error => raise(error)
        }
      ) {
      | LocalException =>
        println("b catch branch 2");
        firstCps(", 55");
        "cc1";
      | error => raise(error)
      }
  );

let async = [@cps] ((it1, itNameCps) => itNameCps(it1));

let res =
  async(
    "0",
    otherCps,
    {
      pub self = this;
      pub onDefer = loc => {
        ();
        /* println("continuation onDefer: " ++ loc); */
        ();
      };
      pub onResume = loc => {
        ();
        /* println("continuation onResume: " ++ loc); */
        ();
      };
      pub onError = loc => {
        ();
        /* println("continuation onError: " ++ loc); */
        ();
      };
      pub resume = value => {
        ();
        println("continuation result is: " ++ value);
        ();
      };
      pub error = value => {
        ();
        switch (value) {
        | _ => println("Error catched!")
        };
        ();
      }
    },
  );

println("::cps finished::");