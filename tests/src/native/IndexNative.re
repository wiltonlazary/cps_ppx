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
        continuation#error(LocalException);
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
        continuation#resume(x ++ "-004");
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

      let x =
        [@defer]
        {
          println("defer branch 2 : res=" ++ ifres);
          continuation#resume(ifres);
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
          "cc222" ++ ifres;
        | error => raise(error)
        };

      let x =
        [@defer]
        {
          ();
          println("***********defer branch xxx : res=" ++ res);
          /* continuation#resume(res); */
          continuation#error(Not_found);
        };

      let cp1 =
        [@o]
        {
          let a = firstCps(x ++ ", 1");

          [@o]
          {
            let a = firstCps(a ++ ", 2");
            a;
          };
        };

      cp1;
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

class type anyClassType = {
  pub inheritance: Hashtbl.t(string, string);
  pub className: string;
  pub instanceof: string => bool
};

type anyObj('a) = {.. selfMethod: string} as 'a;

module type GenericModuleType = {};

module type ClassModuleDefType = {type t;};

module type ClassModuleInheritType = {let classId: string; let className: string; let inheritanceTable: Hashtbl.t(string, string);};

external cast : 'a => 'b = "%identity";

module Any = {
  let classId = __LOC__;
  let className = __MODULE__;
  let inheritanceTable: Hashtbl.t(string, string) = Hashtbl.create(0);
  inheritanceTable |. Hashtbl.add(classId, className);

  class t = {
    as (this: 'this);
    pub inheritance = inheritanceTable;
    pub className = className;
    pub instanceof = targetClassId =>
      try (
        {
          this#inheritance |. Hashtbl.find(targetClassId) |. ignore;
          true;
        }
      ) {
      | _ => false
      };
  };
};

let anyMod: (module ClassModuleInheritType) = (module Any);
module AnyModNew = (val anyMod);

module ClassModule = (Def: ClassModuleDefType, Inherit: ClassModuleInheritType) => {
  let classId = __LOC__;
  let className = __MODULE__;
  let inheritanceTable = Hashtbl.copy(Inherit.inheritanceTable);
  inheritanceTable |. Hashtbl.add(classId, className);
  external cast : 'a => 'b = "%identity";
  include Def;

  class tx = {};
};

module Test =
  ClassModule(
    {
      class t = {
        pub x = "xx";
      };

      let uuu = "hyy";
    },
    Any,
  );

let entityClassName = "entity";
let entityInheritanceTable: Hashtbl.t(string, string) = Hashtbl.create(0);
entityInheritanceTable |. Hashtbl.add(entityClassName, __LOC__);

class entity (v) = {
  as (this: 'this);
  pub inheritance = entityInheritanceTable;
  pub instanceof = className =>
    try (
      {
        this#inheritance |. Hashtbl.find(className);
        true;
      }
    ) {
    | _ => false
    };
  pub kind = "entity";
};

let personClassName = "person";
let personInheritanceTable = Hashtbl.copy(entityInheritanceTable);
personInheritanceTable |. Hashtbl.add(personClassName, __LOC__);

class person = {
  as (this: 'this);
  inherit (class entity)("test") as super;
  pub! inheritance = personInheritanceTable;
  pub! kind = "person";
  pub name = "wilton";
};

let employerClassName = "employer";
let employerInheritanceTable = Hashtbl.copy(personInheritanceTable);
employerInheritanceTable |. Hashtbl.add(employerClassName, __LOC__);

class employer = {
  as (this: 'this);
  inherit class person as super;
  pub! inheritance = employerInheritanceTable;
  pub! kind = "employer";
  pub acount = 10;
};

let painIndexMap: Hashtbl.t(string, person) = Hashtbl.create(10);

let () = {
  open Hashtbl;
  add(painIndexMap, "western paper wasp", new person);
  add(painIndexMap, "sss", (new employer :> person));

  let x: entity = (new person :> entity);

  switch (x) {
  | x when x#instanceof(employerClassName) => print_endline("employer:" ++ string_of_int((cast(x): employer)#acount))
  | x when x#instanceof(personClassName) => print_endline("person:" ++ (cast(x): person)#name)
  | _ => print_endline("other:" ++ (cast(x): entity)#kind)
  };

  ();
};