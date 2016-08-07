;;(set-default-font "Monaco:pixelsize=14")
(set-default-font "mononoki:pixelsize=16")
(defalias 'yes-or-no-p 'y-or-n-p)


(global-set-key "\C-x\C-m" 'execute-extended-command)
(global-set-key "\C-w" 'backward-kill-word)
(global-set-key "\C-h" 'backward-delete-char)
(global-set-key "\C-x\C-k" 'kill-region)
(global-set-key "\C-c\C-k" 'kill-region)
(global-set-key "\C-l" 'goto-line)
(global-set-key [f1] 'help)

(custom-set-variables
 '(ido-mode 1)
 '(ido-everywhere t)
 '(ido-max-prospects 8)
 '(line-number-mode 1)
 '(indent-tabs-mode nil)
 '(blink-cursor-mode nil)
 '(column-number-mode 1)
 '(scroll-bar-mode nil)
 '(tool-bar-mode nil)
 '(menu-bar-mode nil)
 '(mouse-wheel-mode t)
 '(inhibit-startup-screen t)
 '(inhibit-startup-message t)
 '(fill-column 79)
 '(make-backup-files nil))

(show-paren-mode)

;; (add-to-list 'load-path "~/.emacs.d")

(setq package-archives '(("gnu" . "https://elpa.gnu.org/packages/")
                         ("marmalade" . "https://marmalade-repo.org/packages/")
                         ("melpa" . "https://melpa.org/packages/")))

(package-initialize)

;; Setup theme
(setq solarized-use-more-italic t)
(setq solarized-distinct-fringe-background t)
(setq solarized-high-contrast-mode-line t)
(load-theme 'solarized-light t)
(set-foreground-color "#070707")

(require 'solidity-mode)
