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
            println(">> firstErrorCps continuation#error(LocalException);");
            continuation#error(LocalException);
          },
          1000,
        );
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
        Js.Global.setTimeout(
          () => {
            println(">> firstCps continuation#resume(x ++ \"-004\")");
            continuation#resume(x ++ "-004");
          },
          1000,
        );
      }
  );

let boolCps =
  [@cps]
  (
    x =>
      [@defer]
      {
        ();
        println("defer boolCps");
        continuation#resume(x);
      }
  );

let otherCps =
  [@cps]
  (
    x => {
      try (
        try (firstErrorCps(", 1")) {
        | LocalException =>
          println("a catch branch 1");

          let v =
            switch (firstCps("xx1xx2")) {
            | "xx1xx2" as pattern =>
              println("switch: pattern 1 matched: " ++ pattern);
              pattern;
            | other =>
              println("switch: no pattern matched: " ++ other);
              other;
            };

          println("switch result is: " ++ v);
          firstCps(", 55");
          "cc1";
          raise(LocalException);
        | error => raise(error)
        }
      ) {
      | LocalException =>
        println("b catch branch 2");
        "cc1";
      | error => raise(error)
      };

      let ifres =
        if ({
              firstCps(", 55");
              boolCps(false);
            }) {
          println("then branch: 1");
          "then";
        } else {
          println("else branch: 1");
          "else";
        };

      println("ifres: " ++ ifres);

      let v =
        switch (
          {
            let x = firstCps("zazaza");
            let x = x == "zazaza";
            boolCps(x);
          }
        ) {
        | true as pattern =>
          println("switch 2 --- : pattern 1 matched: ");
          pattern;
        | other =>
          println("switch 2 --- : no pattern matched: ");
          other;
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
        | _ => println("//////////Error catched!")
        };
        ();
      }
    },
  );

println("::async finished::");