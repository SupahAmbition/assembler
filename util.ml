(* util.ml: utilities for splitting and file opeartions. All functions
   in this file are already implemented. *)

(* Load a list of strings from a text file. Not particularly efficient
   as it is not tail recursive but gets the job done. *)
let strlist_from_file filename =
  let rec helper inchan =
    try
      let line = input_line inchan in
      line :: (helper inchan)
    with
    | End_of_file ->
       close_in inchan;
       []
  in
  let inchan = open_in filename in
  helper inchan
;;

(* Write a list of strings to file filename. Tail recursive so
   reasonably efficient. *)
let strlist_to_file strlist filename  =
  let rec helper outchan list =
    if list = [] then
      close_out outchan
    else
      begin
        output_string outchan (List.hd list);
        output_string outchan "\n";
        helper outchan (List.tl list);
      end
  in
  let outchan = open_out filename in
  helper outchan strlist
;;


