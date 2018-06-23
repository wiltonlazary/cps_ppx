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
        raise(LocalException);
        println("defer branch 1");
        continuation#resume("001");
        /* continuation#error(Exception); */
      }
  );

let firstCps =
  [@cps]
  (
    x =>
      [@defer]
      {
        ();
        println("defer branch 1");
        continuation#resume("001");
        /* continuation#error(Exception); */
      }
  );

let otherCps =
  [@cps]
  (
    x => {
      try (
        try (firstErrorCps(", 1")) {
        | LocalException =>
          println("catch branch 1");
          firstCps(", 55");
          "cc1";
          raise(LocalException);
        | error => raise(error)
        }
      ) {
      | LocalException =>
        println("catch branch 2");
        "cc1";
      | error => raise(error)
      };

      let res =
        try (
          try (firstErrorCps(", 1")) {
          | LocalException =>
            println("catch branch 1");
            firstCps(", 55");
            "cc1";
            raise(LocalException);
          | error => raise(error)
          }
        ) {
        | LocalException =>
          println("catch branch 2");
          "cc1";
        | error => raise(error)
        };

      [@defer]
      {
        ();
        println("defer branch 2 : res=" ++ res);
        continuation#resume(res);
        /* continuation#error(Not_found); */
      };

      "---";
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