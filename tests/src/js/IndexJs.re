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

[@bs.deriving abstract]
type options = {
  [@bs.optional]
  language: string,
  [@bs.optional]
  session: bool,
  [@bs.optional]
  op: string,
  [@bs.optional]
  processor: string,
  [@bs.optional]
  accept: string,
};

let xx = options(~op="", ());

external cast : 'a => 'b = "%identity";

type entityKind = [ | `Entity];
let entityKindValue = `Entity;

class entity (v: entityKind) = {
  pub tp = v;
  pub kind = "entity";
};

class person = {
  as _;
  inherit (class entity)(`Entity) as super;
  pub! kind = "person";
  pub name = "wilton";
};

class employer = {
  inherit class person as super;
  pub! kind = "employer";
  pub acount = 10;
};

external toEmployer : 'a => employer = "%identity";

let painIndexMap: Hashtbl.t(string, person) = Hashtbl.create(10);

let () = {
  open Hashtbl;
  add(painIndexMap, "western paper wasp", new person);
  add(painIndexMap, "sss", (new employer :> person));

  let x: entity = (new employer :> entity);

  switch (x#kind) {
  | "employer" => print_endline("employer:" ++ string_of_int((cast(x): employer)#acount))
  | "person" => print_endline("person:" ++ (cast(x): person)#name)
  | _ => print_endline("other:" ++ (cast(x): entity)#kind)
  };

  ();
};