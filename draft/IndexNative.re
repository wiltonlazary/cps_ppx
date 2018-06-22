let println = print_endline;
let debugln = value => ();

let firstCps =
  [@cps]
  (
    x =>
      [@defer]
      {
        ();
        continuation#resume(x ++ "-001");
      }
  );

let otherCps =
  [@cps]
  (
    x => {
      firstCps("a" ++ ", 1");
      let a = firstCps(x ++ ", 1");
      let fCps = [@cps] (xCps => xCps(a ++ "c"));
      let a = fCps([@cps] x => x);
      let a = firstCps(a ++ ", 1");

      try (firstCps(a ++ ", 1")) {
      | Not_found => "xxx"
      | error => raise(error)
      };

      let vaab =
        if (1 > 0) {
          println("////////////////// then branch");
          firstCps(a ++ ", 1");
          firstCps(a ++ ", 1");
        } else {
          println("////////////////// else branch");
          [@defer]
          {
            ();
            continuation#resume("001");
          };
          [@defer]
          {
            ();
            continuation#resume("001");
          };
        };

      let b =
        [@defer]
        {
          ();
          continuation#resume("001");
        };

      let a = firstCps(b ++ ", 1vv");

      let cp1 =
        [@o]
        {
          let a = firstCps(a ++ ", 1");

          [@o]
          {
            ();
            firstCps(a ++ ", 2** " ++ a);

            let vaab =
              if (false) {
                println("////////////////// then branch");
                firstCps(a ++ ", 1");
                firstCps(a ++ ", 1");
              } else {
                println("////////////////// else branch");
                [@defer]
                {
                  ();
                  continuation#resume("001");
                };
                [@defer]
                {
                  ();
                  println("defer branch");

                  /* continuation#resume("001"); */
                  continuation#error(Not_found);
                };
              };
            ();
          };
        };

      cp1;
    }
  );

let myFuncCps = [@cps] ((it1, itNameCps) => itNameCps(it1));

let res =
  myFuncCps(
    "0",
    otherCps,
    {
      pub self = this;
      pub willDefer = loc => {
        ();
        /* println("continuation willDefer: " ++ loc); */
        ();
      };
      pub willResume = loc => {
        ();
        /* println("continuation willResume: " ++ loc); */
        ();
      };
      pub willError = loc => {
        ();
        /* println("continuation willError: " ++ loc); */
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