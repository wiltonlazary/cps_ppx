[@ocaml.ppx.context {cookies: []}];
open Parsetree;
open Ast_mapper;
open Ast_helper;
open Asttypes;
open Longident;

let debugln = value => ();

let fail = (loc, txt) => raise(Location.Error(Location.error(~loc, txt)));

let emptyPstr = PStr([]);

let unitExpr = [%expr ()];

let exprToPayload = expr => PStr([Str.mk(Pstr_eval(expr, []))]);

let stringToExpr = s => Exp.mk(Pexp_constant(Const_string(s, None)));

let stringToPayload = s => PStr([Str.mk(Pstr_eval(stringToExpr(s), []))]);

let printExpr = expr => {
  Pprintast.expression(Format.str_formatter, expr);
  debugln(Format.flush_str_formatter());
};

let getExpr = (payload, loc) =>
  Parsetree.(
    switch (payload) {
    | PStr([{pstr_desc: Pstr_eval(expr, _)}]) => expr
    | _ => fail(loc, "must contain an expression")
    }
  );

let endsWith = (fragment, self) => {
  let len = String.length(self);
  len >= String.length(fragment) && String.sub(self, len - 3, 3) == fragment;
};

let skipExpr = expr => Exp.attr(expr, ({txt: "cpsskipped", loc: default_loc^}, emptyPstr));

let leafExpr = (expr, tag) => Exp.attr(expr, ({txt: "cpsleaf", loc: default_loc^}, stringToPayload(tag)));

let continuationExpr = expr => Exp.attr(expr, ({txt: "cpscontinuation", loc: default_loc^}, emptyPstr));

let matchesCpsApply = id =>
  id == "await" || id == "delay" || id |> endsWith("Cps") || id |> endsWith("Await") || id |> endsWith("Delay");

let mkLetExpr = (recFlag, varName, varNameLoc, expr, constraintType, inExpr) =>
  Exp.mk(
    Pexp_let(
      recFlag,
      [
        {
          pvb_pat: {
            ppat_desc: Ppat_var({txt: varName, loc: varNameLoc}),
            ppat_loc: varNameLoc,
            ppat_attributes: [],
          },
          pvb_attributes: [],
          pvb_loc: varNameLoc,
          pvb_expr:
            switch (constraintType) {
            | Some(constraintType) => {pexp_desc: Pexp_constraint(expr, constraintType), pexp_loc: varNameLoc, pexp_attributes: []}
            | None => expr
            },
        },
      ],
      inExpr,
    ),
  );

let mkLetIdentExpr = (recFlag, varName, varNameLoc, identName, constraintType, inExpr) =>
  Exp.mk(
    Pexp_let(
      recFlag,
      [
        {
          pvb_pat: {
            ppat_desc: Ppat_var({txt: varName, loc: varNameLoc}),
            ppat_loc: varNameLoc,
            ppat_attributes: [],
          },
          pvb_attributes: [],
          pvb_loc: varNameLoc,
          pvb_expr:
            switch (constraintType) {
            | Some(constraintType) => {
                pexp_desc:
                  Pexp_constraint(
                    {pexp_desc: Pexp_ident({txt: Lident(identName), loc: varNameLoc}), pexp_loc: varNameLoc, pexp_attributes: []},
                    constraintType,
                  ),
                pexp_loc: varNameLoc,
                pexp_attributes: [],
              }
            | None => {pexp_desc: Pexp_ident({txt: Lident(identName), loc: varNameLoc}), pexp_loc: varNameLoc, pexp_attributes: []}
            },
        },
      ],
      inExpr,
    ),
  );

let mkTryExpr = (expr, caseExprs) => Exp.try_(expr, caseExprs);

let parsers = {
  pri processMatchBranch = (~from="_", branched, matchExpr, caseExprs, nextExpr) => {
    let resumeExprsBranched = ref(0);
    let (resumeExprs, hasResumeExprs) =
      switch (nextExpr) {
      | Some(nextExpr) => (this#descendantParser(~from=__LOC__, nextExpr, resumeExprsBranched), true)
      | None => (unitExpr, false)
      };

    let matchExprBranched = ref(0);
    let matchExpr = this#descendantParser(~from=__LOC__, matchExpr, matchExprBranched);

    let lasCasePattern = ref(None);
    let caseExprsBranched = ref(0);

    let caseExprsComputed =
      caseExprs
      |> List.map(it =>
           switch (it) {
           | {pc_lhs: pattern, pc_guard: guard, pc_rhs: expr} =>
             lasCasePattern := Some(pattern);
             let caseExprBranchedLocal = ref(0);
             let caseExpr = this#descendantParser(~from=__LOC__, expr, caseExprBranchedLocal);
             caseExprsBranched := caseExprsBranched^ + caseExprBranchedLocal^;
             (Exp.case(pattern, ~guard?, caseExpr), caseExprBranchedLocal^ > 0);
           }
         );

    branched := branched^ + resumeExprsBranched^ + matchExprBranched^ + caseExprsBranched^;

    let matchLoc =
      switch (lasCasePattern^) {
      | None => default_loc^
      | Some(pattern) =>
        switch (pattern) {
        | {ppat_loc: loc} => loc
        }
      };

    if (branched^ > 0) {
      let caseExprs =
        caseExprsComputed
        |> List.map(it =>
             switch (it |> fst) {
             | {pc_lhs: pattern, pc_guard: guard, pc_rhs: expr} =>
               if (it |> snd) {
                 Exp.case(
                   pattern,
                   ~guard?,
                   try%expr ([%e expr]) {
                   | error =>
                     _cps_branch_error_(error);
                     _cps_resumed_^ ? `Continued : `Suspended;
                   },
                 );
               } else {
                 Exp.case(
                   pattern,
                   ~guard?,
                   try%expr (
                     {
                       let _cps_result_ = [%e expr];
                       _cps_branch_resume_(_cps_result_);
                       _cps_resumed_^ ? `Continued : `Suspended;
                     }
                   ) {
                   | error =>
                     _cps_branch_error_(error);
                     _cps_resumed_^ ? `Continued : `Suspended;
                   },
                 );
               }
             }
           );

      let%expr _cps_resumed_ = ref(false);
      let _dbg_cps_from = [%e stringToExpr(from)];
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("match cps already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          /*          debugln("match cps resumed: " ++ __LOC__);*/
          if%e (resumeExprsBranched^ == 0) {
            %expr
            if%e (hasResumeExprs) {
              let%expr _cps_result_ = [%e leafExpr(resumeExprs, "r")];
              _cps_branch_resume_(_cps_result_);
            } else {
              %expr
              _cps_branch_resume_(_cps_result_);
            };
          } else {
            continuationExpr(resumeExprs);
          };
          ();
        };
      let _cps_resumed_ = ref(false);
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("match branch cps already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          /*          debugln("match branch cps resumed: " ++ __LOC__);*/
          %e
          Exp.match(~loc=matchLoc, [%expr _cps_result_], caseExprs);
          ();
        };
      if%e (matchExprBranched^ > 0) {
        matchExpr;
      } else {
        let%expr _cps_result_ = [%e matchExpr];
        _cps_branch_resume_(_cps_result_);
        _cps_resumed_^ ? `Continued : `Suspended;
      };
    } else {
      %expr
      if%e (hasResumeExprs) {
        let%expr _cps_result_ = [%e Exp.match(~loc=matchLoc, matchExpr, caseExprs)];
        %e
        resumeExprs;
      } else {
        let%expr _cps_result_ = [%e Exp.match(~loc=matchLoc, matchExpr, caseExprs)];
        _cps_result_;
      };
    };
  };
  pri processIfthenelseBranch = (~from="_", branched, ifExpr, thenExpr, elseExpr, nextExpr) => {
    let resumeExprsBranched = ref(0);
    let (resumeExprs, hasResumeExprs) =
      switch (nextExpr) {
      | Some(nextExpr) => (this#descendantParser(~from=__LOC__, nextExpr, resumeExprsBranched), true)
      | None => (unitExpr, false)
      };

    let ifExprBranched = ref(0);
    let ifExpr = this#descendantParser(~from=__LOC__, ifExpr, ifExprBranched);

    let thenExprBranched = ref(0);
    let thenExpr = this#descendantParser(~from=__LOC__, thenExpr, thenExprBranched);

    let elseExprBranched = ref(0);
    let elseExpr =
      switch (elseExpr) {
      | Some(elseExpr) => this#descendantParser(~from=__LOC__, elseExpr, elseExprBranched)
      | None => unitExpr
      };

    branched := branched^ + resumeExprsBranched^ + ifExprBranched^ + thenExprBranched^ + elseExprBranched^;

    if (branched^ > 0) {
      let thenExpr =
        thenExprBranched^ > 0 ?
          thenExpr :
          {
            let%expr _cps_result_ = [%e thenExpr];
            _cps_branch_resume_(_cps_result_);
            _cps_resumed_^ ? `Continued : `Suspended;
          };

      let elseExpr =
        elseExprBranched^ > 0 ?
          elseExpr :
          {
            let%expr _cps_result_ = [%e elseExpr];
            _cps_branch_resume_(_cps_result_);
            _cps_resumed_^ ? `Continued : `Suspended;
          };

      let%expr _cps_resumed_ = ref(false);
      let _dbg_cps_from = [%e stringToExpr(from)];
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("Ifthenelse cps already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          /*          debugln("Ifthenelse cps branched resumed: " ++ __LOC__);*/

          if%e (resumeExprsBranched^ == 0) {
            %expr
            if%e (hasResumeExprs) {
              let%expr _cps_result_ = [%e leafExpr(resumeExprs, "r")];
              _cps_branch_resume_(_cps_result_);
            } else {
              %expr
              _cps_branch_resume_(_cps_result_);
            };
          } else {
            continuationExpr(resumeExprs);
          };
          ();
        };
      let _cps_resumed_ = ref(false);
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("Ifthenelse branch cps already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          /*          debugln("match branch cps resumed: " ++ __LOC__);*/
          if (_cps_result_) {
            %e
            thenExpr;
          } else {
            %e
            elseExpr;
          };
          ();
        };
      if%e (ifExprBranched^ > 0) {
        ifExpr;
      } else {
        let%expr _cps_result_ = [%e ifExpr];
        _cps_branch_resume_(_cps_result_);
        _cps_resumed_^ ? `Continued : `Suspended;
      };
    } else {
      %expr
      if%e (hasResumeExprs) {
        let%expr _cps_result_ =
          if ([%e ifExpr]) {
            %e
            thenExpr;
          } else {
            %e
            elseExpr;
          };
        %e
        resumeExprs;
      } else {
        let%expr _cps_result_ =
          if ([%e ifExpr]) {
            %e
            thenExpr;
          } else {
            %e
            elseExpr;
          };
        _cps_result_;
      };
    };
  };
  pri processTryBranch = (~from="_", branched, tryExpr, caseExprs, nextExpr) => {
    let resumeExprsBranched = ref(0);
    let (resumeExprs, hasResumeExprs) =
      switch (nextExpr) {
      | Some(nextExpr) => (this#descendantParser(~from=__LOC__, nextExpr, resumeExprsBranched), true)
      | None => (unitExpr, false)
      };

    let tryExprBranched = ref(0);
    let tryExpr = this#descendantParser(~from=__LOC__, tryExpr, tryExprBranched);

    let lasCasePattern = ref(None);
    let caseExprsBranched = ref(0);

    let caseExprsComputed =
      caseExprs
      |> List.map(it =>
           switch (it) {
           | {pc_lhs: pattern, pc_guard: guard, pc_rhs: expr} =>
             lasCasePattern := Some(pattern);
             let caseExprBranchedLocal = ref(0);
             let caseExpr = this#descendantParser(~from=__LOC__, expr, caseExprBranchedLocal);
             caseExprsBranched := caseExprsBranched^ + caseExprBranchedLocal^;
             (Exp.case(pattern, ~guard?, caseExpr), caseExprBranchedLocal^ > 0);
           }
         );

    branched := branched^ + resumeExprsBranched^ + tryExprBranched^ + caseExprsBranched^;

    if (branched^ > 0) {
      let matchLoc =
        switch (lasCasePattern^) {
        | None => default_loc^
        | Some(pattern) =>
          switch (pattern) {
          | {ppat_loc: loc} => loc
          }
        };

      let caseExprs =
        caseExprsComputed
        |> List.map(it =>
             switch (it |> fst) {
             | {pc_lhs: pattern, pc_guard: guard, pc_rhs: expr} =>
               if (it |> snd) {
                 Exp.case(
                   pattern,
                   ~guard?,
                   try%expr ([%e expr]) {
                   | error =>
                     _cps_branch_error_(error);
                     _cps_resumed_^ ? `Continued : `Suspended;
                   },
                 );
               } else {
                 Exp.case(
                   pattern,
                   ~guard?,
                   try%expr (
                     {
                       let _cps_result_ = [%e expr];
                       _cps_branch_resume_(_cps_result_);
                       _cps_resumed_^ ? `Continued : `Suspended;
                     }
                   ) {
                   | error =>
                     _cps_branch_error_(error);
                     _cps_resumed_^ ? `Continued : `Suspended;
                   },
                 );
               }
             }
           );

      let tryExpr =
        tryExprBranched^ > 0 ?
          tryExpr :
          {
            let%expr _cps_result_ = [%e tryExpr];
            _cps_branch_resume_(_cps_result_);
          };

      let%expr _cps_resumed_ = ref(false);
      let _dbg_cps_from = [%e stringToExpr(from)];
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("try cps already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          /*          debugln("try cps resumed: " ++ __LOC__);*/
          if%e (resumeExprsBranched^ == 0) {
            %expr
            if%e (hasResumeExprs) {
              let%expr _cps_result_ = [%e leafExpr(resumeExprs, "r")];
              _cps_branch_resume_(_cps_result_);
            } else {
              %expr
              _cps_branch_resume_(_cps_result_);
            };
          } else {
            continuationExpr(resumeExprs);
          };
          ();
        };
      let _cps_branch_error_ = _cps_error_ =>
        if (_cps_resumed_^) {
          raise(Failure("try cps error already resumed: " ++ __LOC__));
        } else {
          /*          debugln("try cps error resumed: " ++ __LOC__);*/
          %e
          Exp.match(~loc=matchLoc, [%expr _cps_error_], caseExprs);
          ();
        };
      try ([%e tryExpr]) {
      | error =>
        _cps_branch_error_(error);
        _cps_resumed_^ ? `Continued : `Suspended;
      };
    } else {
      %expr
      if%e (hasResumeExprs) {
        let%expr _cps_result_ = [%e mkTryExpr(tryExpr, caseExprs)];
        %e
        resumeExprs;
      } else {
        let%expr _cps_result_ = [%e mkTryExpr(tryExpr, caseExprs)];
        _cps_result_;
      };
    };
  };
  pri processSequenceBranch = (branched, actualSequenceExpr, nextSequenceExpr) =>
    switch (actualSequenceExpr) {
    | {pexp_attributes: [({txt: "defer", loc}, payload)]} =>
      let resumeExprsBranched = ref(0);
      let resumeExprs = this#descendantParser(~from=__LOC__, nextSequenceExpr, resumeExprsBranched);
      this#parseCpsDeferExpr(~from=__LOC__, branched, actualSequenceExpr, loc, resumeExprs, resumeExprsBranched^, false);
    | {pexp_attributes: [({txt: "nocps", loc}, payload)]} => actualSequenceExpr
    | {pexp_attributes: [({txt: "cps", loc}, payload)]} => this#parseCpsFunExpr(actualSequenceExpr, payload, loc)
    | {pexp_desc: Pexp_extension(({txt: "cps", loc}, payload))} => this#parseCpsFunExpr(getExpr(payload, loc), payload, loc)
    | {pexp_desc: Pexp_let(recFlag, bindings, inExpr)} =>
      /*      debugln("WARNING: *********processLetBranch actualSequenceExpr:*********");*/
      this#processLetBranch(branched, recFlag, bindings, inExpr, Some(nextSequenceExpr))
    | {pexp_desc: Pexp_apply({pexp_desc: Pexp_ident({txt: Lident(id), loc})}, expressions)} when matchesCpsApply(id) =>
      let resumeExprsBranched = ref(0);
      let resumeExprs = this#descendantParser(~from=__LOC__, nextSequenceExpr, resumeExprsBranched);
      this#parseCpsApplyExp(~from=__LOC__, branched, id, expressions, loc, resumeExprs, resumeExprsBranched^, false);
    | {pexp_desc: Pexp_ifthenelse(ifExpr, thenExpr, elseExpr)} =>
      this#processIfthenelseBranch(branched, ifExpr, thenExpr, elseExpr, Some(nextSequenceExpr))
    | {pexp_desc: Pexp_try(tryExpr, caseExprs)} => this#processTryBranch(branched, tryExpr, caseExprs, Some(nextSequenceExpr))
    | {pexp_desc: Pexp_match(matchExpr, caseExprs)} => this#processMatchBranch(branched, matchExpr, caseExprs, Some(nextSequenceExpr))
    | other => Exp.mk(Pexp_sequence(other, this#descendantParser(~from=__LOC__, nextSequenceExpr, branched)))
    };
  pri processLetBranch = (branched, recFlag, bindings, inExpr, nextSequenceExpr) =>
    if (List.length(bindings) == 1) {
      let processBindingExpr = (varName, varNameLoc, bindingExpr, constraintType) =>
        switch (bindingExpr) {
        | {pexp_attributes: [({txt: "defer", loc}, payload)]} =>
          let resumeExprsBranched = ref(0);
          let resumeExprs =
            mkLetIdentExpr(
              recFlag,
              varName,
              varNameLoc,
              "_cps_result_",
              constraintType,
              this#descendantParser(~from=__LOC__, inExpr, resumeExprsBranched),
            );
          this#parseCpsDeferExpr(~from=__LOC__, branched, bindingExpr, loc, resumeExprs, resumeExprsBranched^, false);
        | {pexp_attributes: [({txt: "nocps", loc}, payload)]} =>
          mkLetExpr(recFlag, varName, varNameLoc, bindingExpr, constraintType, this#descendantParser(~from=__LOC__, inExpr, branched))
        | {pexp_attributes: [({txt: "cps", loc}, payload)]} =>
          mkLetExpr(
            recFlag,
            varName,
            varNameLoc,
            this#parseCpsFunExpr(bindingExpr, payload, loc),
            constraintType,
            this#descendantParser(~from=__LOC__, inExpr, branched),
          )
        | {pexp_desc: Pexp_extension(({txt: "cps", loc}, payload))} =>
          mkLetExpr(
            recFlag,
            varName,
            varNameLoc,
            this#parseCpsFunExpr(getExpr(payload, loc), payload, loc),
            constraintType,
            this#descendantParser(~from=__LOC__, inExpr, branched),
          )
        | {pexp_desc: Pexp_apply({pexp_desc: Pexp_ident({txt: Lident(id), loc})}, expressions)} when matchesCpsApply(id) =>
          /*          debugln("processLetBranch pvb_pat apply cps single binding: varName = " ++ varName);*/
          let resumeExprsBranched = ref(0);
          let resumeExprs =
            mkLetIdentExpr(
              recFlag,
              varName,
              varNameLoc,
              "_cps_result_",
              constraintType,
              this#descendantParser(~from=__LOC__, inExpr, resumeExprsBranched),
            );
          this#parseCpsApplyExp(~from=__LOC__, branched, id, expressions, loc, resumeExprs, resumeExprsBranched^, false);
        | {pexp_desc: Pexp_ifthenelse(ifExpr, thenExpr, elseExpr)} =>
          /*          debugln("processLetBranch pvb_pat ifthenelse cps single binding: varName = " ++ varName);*/
          let nextExprs = mkLetIdentExpr(recFlag, varName, varNameLoc, "_cps_result_", constraintType, inExpr);
          this#processIfthenelseBranch(branched, ifExpr, thenExpr, elseExpr, Some(nextExprs));
        | {pexp_desc: Pexp_try(tryExpr, caseExprs)} =>
          let nextExprs = mkLetIdentExpr(recFlag, varName, varNameLoc, "_cps_result_", constraintType, inExpr);
          this#processTryBranch(branched, tryExpr, caseExprs, Some(nextExprs));
        | {pexp_desc: Pexp_match(matchExpr, caseExprs)} =>
          let nextExprs = mkLetIdentExpr(recFlag, varName, varNameLoc, "_cps_result_", constraintType, inExpr);
          this#processMatchBranch(branched, matchExpr, caseExprs, Some(nextExprs));
        | other =>
          let bindingExprBranched = ref(0);
          let bindingExpr = this#descendantParser(~from=__LOC__, bindingExpr, bindingExprBranched);

          if (bindingExprBranched^ == 0) {
            /*            debugln("processLetBranch bindingExprBranched == 0, pvb_pat other single binding: varName = " ++ varName);*/
            Exp.mk(
              Pexp_let(recFlag, bindings, this#descendantParser(~from=__LOC__, inExpr, branched)),
            );
          } else {
            /*            debugln("processLetBranch bindingExprBranched > 0, pvb_pat other single binding: varName = " ++ varName);*/
            let resumeExprsBranched = ref(0);
            let resumeExprs =
              mkLetIdentExpr(
                recFlag,
                varName,
                varNameLoc,
                "_cps_result_",
                constraintType,
                this#descendantParser(~from=__LOC__, inExpr, resumeExprsBranched),
              );

            branched := branched^ + bindingExprBranched^ + resumeExprsBranched^;

            if (resumeExprsBranched^ == 0) {
              [@metaloc varNameLoc]
              {
                let%expr _cps_branch_resume_ = {
                  let _cps_resumed_ = ref(false);
                  (
                    _cps_result_ =>
                      if (_cps_resumed_^) {
                        raise(Failure("binding cps direct already resumed: " ++ __LOC__));
                      } else {
                        _cps_resumed_ := true;
                        /*                        debugln("binding cps direct resumed: " ++ __LOC__);*/
                        let _cps_result_ = [%e resumeExprs];
                        _cps_branch_resume_(_cps_result_);
                        ();
                      }
                  );
                };
                %e
                bindingExpr;
              };
            } else {
              [@metaloc varNameLoc]
              {
                let%expr _cps_branch_resume_ = {
                  let _cps_resumed_ = ref(false);
                  (
                    _cps_result_ =>
                      if (_cps_resumed_^) {
                        raise(Failure("binding cps branched already resumed: " ++ __LOC__));
                      } else {
                        _cps_resumed_ := true;
                        /*                        debugln("binding cps branched resumed: " ++ __LOC__);*/
                        %e
                        resumeExprs;
                        ();
                      }
                  );
                };
                %e
                bindingExpr;
              };
            };
          };
        };

      switch (List.hd(bindings)) {
      | {pvb_pat: {ppat_desc: Ppat_var({txt: varName, loc: varNameLoc})}, pvb_expr: bindingExpr} =>
        switch (bindingExpr) {
        | {pexp_desc: Pexp_constraint(bindingExpr, constraintType)} =>
          processBindingExpr(varName, varNameLoc, bindingExpr, Some(constraintType))
        | other => processBindingExpr(varName, varNameLoc, other, None)
        }
      | other =>
        /*        debugln("processLetBranch other single binding:");*/
        skipExpr(Exp.mk(Pexp_let(recFlag, bindings, this#descendantParser(~from=__LOC__, inExpr, branched))))
      };
    } else {
      /*      debugln("processLetBranch multi bindings:");*/
      skipExpr(
        Exp.mk(Pexp_let(recFlag, bindings, this#descendantParser(~from=__LOC__, inExpr, branched))),
      );
    };
  pri parseCpsApplyExp = (~from="_", branched, id, expressions, loc, resumeExprs, resumeExprsBranched, terminator) => {
    /*    debugln("ppx apply cps:");*/
    branched := branched^ + 1 + resumeExprsBranched;

    let expressions =
      expressions
      |> List.map(it => {
           let expr = snd(it);
           switch (expr) {
           | {pexp_attributes: [({txt: "cps", loc}, payload)]} => (fst(it), this#parseCpsFunExpr(expr, payload, loc))
           | {pexp_desc: Pexp_extension(({txt: "cps", loc}, payload))} => (
               fst(it),
               this#parseCpsFunExpr(getExpr(payload, loc), payload, loc),
             )
           | other => it
           };
         })
      |> List.append(_, [("", [%expr _cps_continuation_])]);

    let applyExpr =
      Exp.attr(Exp.mk(Pexp_apply(Exp.ident({txt: Lident(id), loc}), expressions)), ({txt: "cpsapply", loc}, emptyPstr));

    [@metaloc loc]
    {
      let%expr _cps_resumed_ = ref(false);
      let _dbg_cps_from = [%e stringToExpr(from)];
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("apply cps branched already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          /*          debugln("apply cps branched resumed: " ++ __LOC__);*/
          if%e (terminator) {
            leafExpr(resumeExprs, "t");
          } else if (resumeExprsBranched == 0) {
            let%expr _cps_result_ = [%e leafExpr(resumeExprs, "r")];
            _cps_branch_resume_(_cps_result_);
          } else {
            continuationExpr(resumeExprs);
          };
          ();
        };
      let _cps_continuation_ = {
        pub self = this;
        pub resumed = () => _cps_resumed_^;
        pub resume = _cps_branch_resume_;
        pub error = _cps_branch_error_;
        pub onDefer = _cps_continuation_#onDefer;
        pub onResume = _cps_continuation_#onResume;
        pub onError = _cps_continuation_#onError
      };
      %e
      applyExpr;
    };
  };
  pri parseCpsDeferExpr = (~from="_", branched, deferExpr, loc, resumeExprs, resumeExprsBranched, terminator) => {
    /*    debugln("ppx defer cps:");*/
    branched := branched^ + 1 + resumeExprsBranched;
    let deferExpr = Exp.attr(deferExpr, ({txt: "cpsdefered", loc}, emptyPstr));

    [@metaloc loc]
    {
      let%expr _cps_resumed_ = ref(false);
      let _cps_error_resumed_ = ref(false);
      let _dbg_cps_from = [%e stringToExpr(from)];

      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("defer cps already resumed: " ++ __LOC__));
        } else {
          _cps_continuation_#onResume(__LOC__);
          _cps_resumed_ := true;
          /*          debugln("defer cps resumed: " ++ __LOC__);*/
          if%e (terminator) {
            leafExpr(resumeExprs, "t");
          } else if (resumeExprsBranched == 0) {
            let%expr _cps_result_ = [%e leafExpr(resumeExprs, "r")];
            _cps_branch_resume_(_cps_result_);
          } else {
            continuationExpr(resumeExprs);
          };
          ();
        };
      let _cps_branch_error_ = _cps_error_ =>
        if (_cps_error_resumed_^) {
          raise(Failure("defer cps error already resumed: " ++ __LOC__));
        } else {
          _cps_continuation_#onError(__LOC__);
          _cps_error_resumed_ := true;
          /*          debugln("defer cps error resumed: " ++ __LOC__);*/
          _cps_branch_error_(_cps_error_);
          ();
        };
      let continuation = {
        pub self = this;
        pub resumed = () => _cps_resumed_^;
        pub resume = _cps_branch_resume_;
        pub error = _cps_branch_error_
      };
      _cps_continuation_#onDefer(__LOC__);
      %e
      deferExpr;
      _cps_resumed_^ ? `Continued : `Suspended;
    };
  };
  pri descendantParser = (~from="_", expr, branched) =>
    switch (expr) {
    | {pexp_attributes: [({txt: "defer", loc}, payload)]} =>
      this#parseCpsDeferExpr(~from=__LOC__, branched, expr, loc, [%expr _cps_branch_resume_(_cps_result_)], 0, true)
    | {pexp_attributes: [({txt: "nocps", loc}, payload)]} => expr
    | {pexp_attributes: [({txt: "cps", loc}, payload)]} => this#parseCpsFunExpr(expr, payload, loc)
    | {pexp_desc: Pexp_extension(({txt: "cps", loc}, payload))} => this#parseCpsFunExpr(getExpr(payload, loc), payload, loc)
    | {pexp_desc: Pexp_sequence(actualSequenceExpr, nextSequenceExpr)} =>
      this#processSequenceBranch(branched, actualSequenceExpr, nextSequenceExpr)
    | {pexp_desc: Pexp_let(recFlag, bindings, inExpr)} => this#processLetBranch(branched, recFlag, bindings, inExpr, None)
    | {pexp_desc: Pexp_apply({pexp_desc: Pexp_ident({txt: Lident(id), loc})}, expressions)} when matchesCpsApply(id) =>
      this#parseCpsApplyExp(~from=__LOC__, branched, id, expressions, loc, [%expr _cps_branch_resume_(_cps_result_)], 0, true)
    | {pexp_desc: Pexp_ifthenelse(ifExpr, thenExpr, elseExpr)} =>
      this#processIfthenelseBranch(branched, ifExpr, thenExpr, elseExpr, None)
    | {pexp_desc: Pexp_try(tryExpr, caseExprs)} => this#processTryBranch(branched, tryExpr, caseExprs, None)
    | {pexp_desc: Pexp_match(matchExpr, caseExprs)} => this#processMatchBranch(branched, matchExpr, caseExprs, None)
    | other =>
      switch (other) {
      | {pexp_desc: Pexp_constant(constant)} => other
      | other => skipExpr(other)
      }
    };
  pri parseCpsFunExpr = (expr, payload, loc) => {
    /*    debugln("ppx parseCpsFunExpr:");*/

    let parseCpsFuncBodyExpr = (expr, loc) => {
      let branched = ref(0);
      let expr = this#descendantParser(~from=__LOC__, expr, branched);

      if (branched^ == 0) {
        [@metaloc loc]
        {
          let%expr _cps_result_ = [%e expr];
          _cps_continuation_#resume(_cps_result_);
          `Continued;
        };
      } else {
        [@metaloc loc]
        {
          let%expr _cps_resumed_ = ref(false);
          let _cps_branch_error_ = _cps_error_ =>
            if (_cps_resumed_^) {
              raise(Failure("cpsfun error already resumed: " ++ __LOC__));
            } else {
              _cps_resumed_ := true;
              /*              debugln("cpsfun error resumed: " ++ __LOC__);*/
              _cps_continuation_#error(_cps_error_);
              ();
            };
          let _cps_branch_resume_ = _cps_result_ =>
            if (_cps_resumed_^) {
              raise(Failure("cpsfun already resumed: " ++ __LOC__));
            } else {
              _cps_resumed_ := true;
              /*              debugln("cpsfun resumed: " ++ __LOC__);*/
              _cps_continuation_#resume(_cps_result_);
              ();
            };
          %e
          expr;
        };
      };
    };

    let rec buildFunExp = (bodyExp, patDescs) =>
      switch (patDescs) {
      | [] => bodyExp
      | [head, ...tail] => buildFunExp([%expr ([%p head]) => [%e bodyExp]], tail)
      };

    let rec accFunExprs = (expr, patDescs) =>
      switch (expr) {
      | {pexp_desc: Pexp_fun(label, _, patDesc, pexpDesc)} => accFunExprs(pexpDesc, [patDesc, ...patDescs])
      | other => buildFunExp([%expr _cps_continuation_ => [%e parseCpsFuncBodyExpr(other, loc)]], patDescs)
      };

    let resExpr = Exp.attr([@metaloc loc] [%expr [%e accFunExprs(expr, [])]], ({txt: "cpsfun", loc}, emptyPstr));
    printExpr(resExpr);
    resExpr;
  };
  pub parseNode = expr =>
    switch (expr) {
    | {pexp_attributes: [({txt: "cpsfun", loc}, payload)]} => expr
    | {pexp_attributes: [({txt: "nocps", loc}, payload)]} => expr
    | {pexp_attributes: [({txt: "cps", loc}, payload)]} => this#parseCpsFunExpr(expr, payload, loc)
    | {pexp_desc: Pexp_extension(({txt: "cps", loc}, payload))} => this#parseCpsFunExpr(getExpr(payload, loc), payload, loc)
    | other => other
    }
};

let cpsMapper = argv => {...default_mapper, expr: (mapper, expr) => default_mapper.expr(mapper, parsers#parseNode(expr))};

let () = register("cps_ppx", cpsMapper);