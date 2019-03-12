
#!begin_codebook

alias g++='g++ -g -Wall -Wshadow -DCDEBUG' #.basrc
alias a='setxkbmap us -option'
alias m='setxkbmap us -option caps:escape'
alias ma='setxkbmap us -variant dvp -option caps:escape'

#settings
gsettings set org.compiz.core:/org/compiz/profiles/Default/plugins/core/ hsize 4
gsettings set org.gnome.desktop.wm.preferences focus-mode 'sloppy'

set si cin #.vimrc
set ts=4 sw=4 noet
set cb=unnamed

(global-set-key (kbd "C-x <next>") 'other-window) #.emacs
(global-set-key (kbd "C-x <prior>") 'previous-multiframe-window)
(global-set-key (kbd "C-M-z") 'ansi-term)

(global-linum-mode 1)
(column-number-mode 1)
(show-paren-mode 1)
(setq-default indent-tabs-mode nil)


valgrind --vgdb-error=0 ./a <inp & #valgrind
gdb a
target remote | vgdb

#!end_codebook
