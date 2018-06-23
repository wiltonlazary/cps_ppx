[@ocaml.ppx.context {cookies: []}];
let println = print_endline;
let debugln = value => ();

exception Exception;
exception LocalException;

let firstErrorCps =
  [@cpsfun]
  (
    (x, _cps_continuation_) => {
      let _cps_resumed_ = ref(false);
      let _cps_branch_error_ = _cps_error_ =>
        if (_cps_resumed_^) {
          raise(Failure("cpsfun error already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          _cps_continuation_#error(_cps_error_);
          ();
        };
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("cpsfun already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          _cps_continuation_#resume(_cps_result_);
          ();
        };

      let _cps_resumed_ = ref(false);
      let _dbg_cps_from = "File \"src/Cps_ppx.re\", line 723, characters 35-42";
      let _cps_branch_error_ = _cps_error_ =>
        if (_cps_resumed_^) {
          raise(Failure("defer cps error already resumed: " ++ __LOC__));
        } else {
          _cps_continuation_#onError(__LOC__);
          _cps_resumed_ := true;
          _cps_branch_error_(_cps_error_);
          ();
        };
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("defer cps already resumed: " ++ __LOC__));
        } else {
          _cps_continuation_#onResume(__LOC__);
          _cps_resumed_ := true;
          [@cpsleaf "t"] _cps_branch_resume_(_cps_result_);

          ();
        };
      let continuation = {
        pub self = this;
        pub resumed = () => _cps_resumed_^;
        pub resume = _cps_branch_resume_;
        pub error = _cps_branch_error_
      };
      _cps_continuation_#onDefer(__LOC__);
      [@defer]
      [@cpsdefered]
      {
        ();
        raise(LocalException);
        println("defer branch 1");
        continuation#resume("001");
      };
      _cps_resumed_^ ? `Continued : `Suspended;
    }
  );

let firstCps =
  [@cpsfun]
  (
    (x, _cps_continuation_) => {
      let _cps_resumed_ = ref(false);
      let _cps_branch_error_ = _cps_error_ =>
        if (_cps_resumed_^) {
          raise(Failure("cpsfun error already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          _cps_continuation_#error(_cps_error_);
          ();
        };
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("cpsfun already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          _cps_continuation_#resume(_cps_result_);
          ();
        };

      let _cps_resumed_ = ref(false);
      let _dbg_cps_from = "File \"src/Cps_ppx.re\", line 723, characters 35-42";
      let _cps_branch_error_ = _cps_error_ =>
        if (_cps_resumed_^) {
          raise(Failure("defer cps error already resumed: " ++ __LOC__));
        } else {
          _cps_continuation_#onError(__LOC__);
          _cps_resumed_ := true;
          _cps_branch_error_(_cps_error_);
          ();
        };
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("defer cps already resumed: " ++ __LOC__));
        } else {
          _cps_continuation_#onResume(__LOC__);
          _cps_resumed_ := true;
          [@cpsleaf "t"] _cps_branch_resume_(_cps_result_);

          ();
        };
      let continuation = {
        pub self = this;
        pub resumed = () => _cps_resumed_^;
        pub resume = _cps_branch_resume_;
        pub error = _cps_branch_error_
      };
      _cps_continuation_#onDefer(__LOC__);
      [@defer]
      [@cpsdefered]
      {
        ();
        println("defer branch 1");
        continuation#resume(x);
      };
      _cps_resumed_^ ? `Continued : `Suspended;
    }
  );

let otherCps =
  [@cpsfun]
  (
    (x, _cps_continuation_) => {
      let _cps_resumed_ = ref(false);
      let _cps_branch_error_ = _cps_error_ =>
        if (_cps_resumed_^) {
          raise(Failure("cpsfun error already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          _cps_continuation_#error(_cps_error_);
          ();
        };
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("cpsfun already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          _cps_continuation_#resume(_cps_result_);
          ();
        };
      let _cps_resumed_ = ref(false);
      let _dbg_cps_from = "_";
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("try cps already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          [@cpscontinuation]
          {
            let _cps_resumed_ = ref(false);
            let _dbg_cps_from = "_";
            let _cps_branch_resume_ = _cps_result_ =>
              if (_cps_resumed_^) {
                raise(Failure("try cps already resumed: " ++ __LOC__));
              } else {
                _cps_resumed_ := true;
                [@cpscontinuation]
                {
                  let res = _cps_result_;

                  let _cps_resumed_ = ref(false);
                  let _dbg_cps_from = "File \"src/Cps_ppx.re\", line 454, characters 35-42";
                  let _cps_branch_error_ = _cps_error_ =>
                    if (_cps_resumed_^) {
                      raise(Failure("defer cps error already resumed: " ++ __LOC__));
                    } else {
                      _cps_continuation_#onError(__LOC__);
                      _cps_resumed_ := true;
                      _cps_branch_error_(_cps_error_);
                      ();
                    };
                  let _cps_branch_resume_ = _cps_result_ =>
                    if (_cps_resumed_^) {
                      raise(Failure("defer cps already resumed: " ++ __LOC__));
                    } else {
                      _cps_continuation_#onResume(__LOC__);
                      _cps_resumed_ := true;
                      {
                        let _cps_resumed_ = [@cpsleaf "r"] "---";
                        _cps_branch_resume_(_cps_result_);
                      };
                      ();
                    };
                  let continuation = {
                    pub self = this;
                    pub resumed = () => _cps_resumed_^;
                    pub resume = _cps_branch_resume_;
                    pub error = _cps_branch_error_
                  };
                  _cps_continuation_#onDefer(__LOC__);
                  [@defer]
                  [@cpsdefered]
                  {
                    ();
                    println("defer branch 2 : res=" ++ res);
                    continuation#resume(res);
                  };
                  _cps_resumed_^ ? `Continued : `Suspended;
                };
                ();
              };
            let _cps_branch_error_ = _cps_error_ =>
              if (_cps_resumed_^) {
                raise(Failure("try cps error already resumed: " ++ __LOC__));
              } else {
                switch (_cps_error_) {

                | LocalException =>
                  try (
                    {
                      let _cps_result_ = {
                        println("catch branch 2");
                        "cc1";
                      };
                      _cps_branch_resume_(_cps_result_);
                      _cps_resumed_^ ? `Continued : `Suspended;
                    }
                  ) {
                  | error =>
                    _cps_branch_error_(error);
                    _cps_resumed_^ ? `Continued : `Suspended;
                  }

                | error =>
                  try (
                    {
                      let _cps_result_ = [@cpsskipped] raise(error);
                      _cps_branch_resume_(_cps_result_);
                      _cps_resumed_^ ? `Continued : `Suspended;
                    }
                  ) {
                  | error =>
                    _cps_branch_error_(error);
                    _cps_resumed_^ ? `Continued : `Suspended;
                  }
                };
                ();
              };
            try (
              {
                let _cps_resumed_ = ref(false);
                let _dbg_cps_from = "_";
                let _cps_branch_resume_ = _cps_result_ =>
                  if (_cps_resumed_^) {
                    raise(Failure("try cps already resumed: " ++ __LOC__));
                  } else {
                    _cps_resumed_ := true;
                    {
                      let _cps_resumed_ = [@cpsleaf "r"] ();
                      _cps_branch_resume_(_cps_result_);
                    };
                    ();
                  };
                let _cps_branch_error_ = _cps_error_ =>
                  if (_cps_resumed_^) {
                    raise(Failure("try cps error already resumed: " ++ __LOC__));
                  } else {
                    switch (_cps_error_) {

                    | LocalException =>
                      try (
                        {
                          println("catch branch 1");
                          let _cps_resumed_ = ref(false);
                          let _dbg_cps_from = "File \"src/Cps_ppx.re\", line 464, characters 34-41";
                          let _cps_branch_resume_ = _cps_result_ =>
                            if (_cps_resumed_^) {
                              raise(Failure("apply cps branched already resumed: " ++ __LOC__));
                            } else {
                              _cps_resumed_ := true;
                              {
                                let _cps_resumed_ =
                                  [@cpsleaf "r"]
                                  {
                                    "cc1";
                                    [@cpsskipped] raise(LocalException);
                                  };
                                _cps_branch_resume_(_cps_result_);
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
                          [@cpsapply] firstCps(", 55", _cps_continuation_);
                        }
                      ) {
                      | error =>
                        _cps_branch_error_(error);
                        _cps_resumed_^ ? `Continued : `Suspended;
                      }

                    | error =>
                      try (
                        {
                          let _cps_result_ = [@cpsskipped] raise(error);
                          _cps_branch_resume_(_cps_result_);
                          _cps_resumed_^ ? `Continued : `Suspended;
                        }
                      ) {
                      | error =>
                        _cps_branch_error_(error);
                        _cps_resumed_^ ? `Continued : `Suspended;
                      }
                    };
                    ();
                  };
                try (
                  {
                    let _cps_resumed_ = ref(false);
                    let _dbg_cps_from = "File \"src/Cps_ppx.re\", line 731, characters 34-41";
                    let _cps_branch_resume_ = _cps_result_ =>
                      if (_cps_resumed_^) {
                        raise(Failure("apply cps branched already resumed: " ++ __LOC__));
                      } else {
                        _cps_resumed_ := true;
                        [@cpsleaf "t"] _cps_branch_resume_(_cps_result_);

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
                    [@cpsapply] firstErrorCps(", 1", _cps_continuation_);
                  }
                ) {
                | error =>
                  _cps_branch_error_(error);
                  _cps_resumed_^ ? `Continued : `Suspended;
                };
              }
            ) {
            | error =>
              _cps_branch_error_(error);
              _cps_resumed_^ ? `Continued : `Suspended;
            };
          };
          ();
        };
      let _cps_branch_error_ = _cps_error_ =>
        if (_cps_resumed_^) {
          raise(Failure("try cps error already resumed: " ++ __LOC__));
        } else {
          switch (_cps_error_) {

          | LocalException =>
            try (
              {
                let _cps_result_ = {
                  println("catch branch 2");
                  "cc1";
                };
                _cps_branch_resume_(_cps_result_);
                _cps_resumed_^ ? `Continued : `Suspended;
              }
            ) {
            | error =>
              _cps_branch_error_(error);
              _cps_resumed_^ ? `Continued : `Suspended;
            }

          | error =>
            try (
              {
                let _cps_result_ = [@cpsskipped] raise(error);
                _cps_branch_resume_(_cps_result_);
                _cps_resumed_^ ? `Continued : `Suspended;
              }
            ) {
            | error =>
              _cps_branch_error_(error);
              _cps_resumed_^ ? `Continued : `Suspended;
            }
          };
          ();
        };
      try (
        {
          let _cps_resumed_ = ref(false);
          let _dbg_cps_from = "_";
          let _cps_branch_resume_ = _cps_result_ =>
            if (_cps_resumed_^) {
              raise(Failure("try cps already resumed: " ++ __LOC__));
            } else {
              _cps_resumed_ := true;
              {
                let _cps_resumed_ = [@cpsleaf "r"] ();
                _cps_branch_resume_(_cps_result_);
              };
              ();
            };
          let _cps_branch_error_ = _cps_error_ =>
            if (_cps_resumed_^) {
              raise(Failure("try cps error already resumed: " ++ __LOC__));
            } else {
              switch (_cps_error_) {

              | LocalException =>
                try (
                  {
                    println("catch branch 1");
                    let _cps_resumed_ = ref(false);
                    let _dbg_cps_from = "_";
                    let _cps_branch_resume_ = _cps_result_ =>
                      if (_cps_resumed_^) {
                        raise(Failure("match cps already resumed: " ++ __LOC__));
                      } else {
                        _cps_resumed_ := true;
                        [@cpscontinuation]
                        {
                          let v = _cps_result_;

                          println("switch result is: " ++ v);
                          let _cps_resumed_ = ref(false);
                          let _dbg_cps_from = "File \"src/Cps_ppx.re\", line 464, characters 34-41";
                          let _cps_branch_resume_ = _cps_result_ =>
                            if (_cps_resumed_^) {
                              raise(Failure("apply cps branched already resumed: " ++ __LOC__));
                            } else {
                              _cps_resumed_ := true;
                              {
                                let _cps_resumed_ =
                                  [@cpsleaf "r"]
                                  {
                                    "cc1";
                                    [@cpsskipped] raise(LocalException);
                                  };
                                _cps_branch_resume_(_cps_result_);
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
                          [@cpsapply] firstCps(", 55", _cps_continuation_);
                        };
                        ();
                      };
                    let _cps_resumed_ = ref(false);
                    let _cps_branch_resume_ = _cps_result_ =>
                      if (_cps_resumed_^) {
                        raise(Failure("match branch cps already resumed: " ++ __LOC__));
                      } else {
                        _cps_resumed_ := true;

                        switch (_cps_result_) {

                        | "xx1xx2" as pattern =>
                          try (
                            {
                              let _cps_result_ = {
                                println("switch: pattern 1 matched: " ++ pattern);
                                [@cpsskipped] pattern;
                              };
                              _cps_branch_resume_(_cps_result_);
                              _cps_resumed_^ ? `Continued : `Suspended;
                            }
                          ) {
                          | error =>
                            _cps_branch_error_(error);
                            _cps_resumed_^ ? `Continued : `Suspended;
                          }

                        | other =>
                          try (
                            {
                              let _cps_result_ = {
                                println("switch: no pattern matched: " ++ other);
                                [@cpsskipped] other;
                              };
                              _cps_branch_resume_(_cps_result_);
                              _cps_resumed_^ ? `Continued : `Suspended;
                            }
                          ) {
                          | error =>
                            _cps_branch_error_(error);
                            _cps_resumed_^ ? `Continued : `Suspended;
                          }
                        };
                        ();
                      };

                    let _cps_resumed_ = ref(false);
                    let _dbg_cps_from = "File \"src/Cps_ppx.re\", line 731, characters 34-41";
                    let _cps_branch_resume_ = _cps_result_ =>
                      if (_cps_resumed_^) {
                        raise(Failure("apply cps branched already resumed: " ++ __LOC__));
                      } else {
                        _cps_resumed_ := true;
                        [@cpsleaf "t"] _cps_branch_resume_(_cps_result_);

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
                    [@cpsapply] firstCps("xx1xx2", _cps_continuation_);
                  }
                ) {
                | error =>
                  _cps_branch_error_(error);
                  _cps_resumed_^ ? `Continued : `Suspended;
                }

              | error =>
                try (
                  {
                    let _cps_result_ = [@cpsskipped] raise(error);
                    _cps_branch_resume_(_cps_result_);
                    _cps_resumed_^ ? `Continued : `Suspended;
                  }
                ) {
                | error =>
                  _cps_branch_error_(error);
                  _cps_resumed_^ ? `Continued : `Suspended;
                }
              };
              ();
            };
          try (
            {
              let _cps_resumed_ = ref(false);
              let _dbg_cps_from = "File \"src/Cps_ppx.re\", line 731, characters 34-41";
              let _cps_branch_resume_ = _cps_result_ =>
                if (_cps_resumed_^) {
                  raise(Failure("apply cps branched already resumed: " ++ __LOC__));
                } else {
                  _cps_resumed_ := true;
                  [@cpsleaf "t"] _cps_branch_resume_(_cps_result_);

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
              [@cpsapply] firstErrorCps(", 1", _cps_continuation_);
            }
          ) {
          | error =>
            _cps_branch_error_(error);
            _cps_resumed_^ ? `Continued : `Suspended;
          };
        }
      ) {
      | error =>
        _cps_branch_error_(error);
        _cps_resumed_^ ? `Continued : `Suspended;
      };
    }
  );

let async =
  [@cpsfun]
  (
    (it1, itNameCps, _cps_continuation_) => {
      let _cps_resumed_ = ref(false);
      let _cps_branch_error_ = _cps_error_ =>
        if (_cps_resumed_^) {
          raise(Failure("cpsfun error already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          _cps_continuation_#error(_cps_error_);
          ();
        };
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("cpsfun already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          _cps_continuation_#resume(_cps_result_);
          ();
        };
      let _cps_resumed_ = ref(false);
      let _dbg_cps_from = "File \"src/Cps_ppx.re\", line 731, characters 34-41";
      let _cps_branch_resume_ = _cps_result_ =>
        if (_cps_resumed_^) {
          raise(Failure("apply cps branched already resumed: " ++ __LOC__));
        } else {
          _cps_resumed_ := true;
          [@cpsleaf "t"] _cps_branch_resume_(_cps_result_);

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
      [@cpsapply] itNameCps(it1, _cps_continuation_);
    }
  );

let res =
  async(
    "0",
    otherCps,
    {
      pub self = this;
      pub onDefer = loc => {
        ();

        ();
      };
      pub onResume = loc => {
        ();

        ();
      };
      pub onError = loc => {
        ();

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
