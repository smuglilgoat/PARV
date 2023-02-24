;;;;;;;;;;;;;;;;;;;;;;;;
;;; Setup Procedures ;;;
;;;;;;;;;;;;;;;;;;;;;;;;

breed [sheeps sheep] ;; sheeps pour parler de l’ensemble des moutons
breed [wolves wolf]
sheeps-own [hunger]
wolves-own [hunger energy] ;; Les loups ont un attribut d ́eterminant leur faim
to setup ;; Initialise la simulation
  clear-all ;; Remet la simulation `a z ́ero
  set-default-shape wolves "wolf"
  create-wolves 10 ;; Cr ́ee 50 tortues de l’esp`ece loup
  ask wolves 
  [ set color gray  
    fd 50 
    set energy random 100]
  set-default-shape wolves "sheeps"
  create-sheeps 50
  [ set color brown  
    fd 50 ]
  ask patches
    [ set pcolor green ] ;; of the view turn green
  ask patches
  [ if pxcor = 0 and pycor = 0         ;; patches on the right side
        [ set pcolor yellow] ]  ;; of the view turn green
  reset-ticks ;; Remet l’horloge de la simulation `a 0
end

to go
  ask wolves [
    if energy > 0
    [fd 1            ;; forward 1 step
    rt random 10    ;; turn right
    lt random 10    ;; turn left
    set energy energy - 1]
  ]
  
  ask sheeps [
    fd 1            ;; forward 1 step
    rt random 10    ;; turn right
    lt random 10    ;; turn left
  ]
  tick
end



;;;;;;;;;;;;;;;;;;;;;;;
;;; Main Procedures ;;;
;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;
;;; Layout ;;;
;;;;;;;;;;;;;;


; Copyright 2005 Uri Wilensky.
; See Info tab for full copyright and license.