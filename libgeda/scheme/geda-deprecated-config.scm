;; gEDA - GPL Electronic Design Automation
;; gschem - gEDA Schematic Capture - Scheme API
;; Copyright (C) 2012 Peter Brett <peter@peter-b.co.uk>
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
;;

;; This file contains deprecated configuration functions for RC files.
;; The use gEDA RC files for configuration is being replaced by the
;; use of key-value files that are parsed rather than executed. This
;; will allow configuration to be safely written back to disk, and
;; will have security benefits.  The functions defined in this file
;; are intended for use in legacy RC files during the transition
;; process.

(or (defined? 'define-syntax)
    (use-modules (ice-9 syncase)))

;; ===================================================================
;; Utility functions and macros
;; ===================================================================

;; Returns an RC function closure to replace the legacy configuration
;; function OLD-ID.  The returned closure takes an arbitrary number of
;; arguments, and does nothing other than print a deprecation message
;; the first time it is called.
(define (%rc-dead-config old-id)
  ;; FIXME more helpful error message with link to documentation.
  (define (deprecation-warning)
    (format (current-error-port)
"The RC file function '~A' is deprecated and does nothing.

RC configuration functions will be removed in an upcoming gEDA
release.  Please use configuration files instead.

" old-id))
  (let ((warned? #f))
    (lambda args
      (or warned? (begin (deprecation-warning) (set! warned? #t))))))

;; Convenience macro for using %rc-dead-config.
;;
;;   %define-rc-dead-config OLD-ID
;;
;; Creates a dead rc configuration function called OLD-ID.
(define-syntax %define-rc-dead-config
  (syntax-rules ()
    ((_ old-id)
     (define old-id (%rc-dead-config (quote old-id))))))

;; Returns an RC function closure to replace the legacy configuration
;; function OLD-ID. The returned closure takes an arbitrary number of
;; arguments, and sets the configuration parameter determined by GROUP
;; and KEY to the result of passing its arguments to
;; VALUE-TRANSFORMER.  The first time the closure is called, it prints
;; a deprecation message.
(define (%rc-deprecated-config old-id group key value-transformer)
  ;; FIXME more helpful error message with link to documentation.
  (define (deprecation-warning)
    (format (current-error-port)
"The RC file function '~A' is deprecated.

RC configuration functions will be removed in an upcoming gEDA
release.  Please use configuration files instead.

" old-id))
  (let ((warned? #f))
    (lambda args
      (or warned?
          (begin (deprecation-warning) (set! warned? #t)))
      ((@ (geda config) config-set!)
       (rc-config) group key (apply value-transformer args)))))

;; Convenience macro for using %rc-deprecated-config.
;;
;;   %define-rc-deprecated-config OLD-ID GROUP KEY VALUE-TRANSFORMER
;;
;; Creates a deprecated rc configuration function called OLD-ID that
;; uses VALUE-TRANSFORMER to set the configuration parameter by GROUP
;; and KEY.
(define-syntax %define-rc-deprecated-config
  (syntax-rules ()
    ((_ old-id group key value-transformer)
     (define old-id (%rc-deprecated-config (quote old-id) group key
                                           value-transformer)))))

;; Identity value transformer for %define-rc-deprecated-config
(define (%rc-deprecated-string-transformer str) str)