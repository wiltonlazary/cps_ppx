#!/bin/bash
ocamlfind ppx_tools/dumpast -loc_keep -attrs_keep draft/draft_ml.ml | tail --lines=+3 | head --lines=-1
#| refmt --print-width=140 --parse=ml > draft/draft_ml_ast.re