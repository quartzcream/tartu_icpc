
#!begin_codebook

alias g++='g++ -g -Wall -Wshadow -Wconversion \  #.bashrc
    -fsanitize=undefined,address -DCDEBUG'       #.bashrc
alias a='setxkbmap us -option'                   #.bashrc
alias m='setxkbmap us -option caps:escape'       #.bashrc
alias ma='setxkbmap us -variant dvp \            #.bashrc
    -option caps:escape'                         #.bashrc

gsettings set org.compiz.core: \                #settings
    /org/compiz/profiles/Default/plugins/core/ hsize 4
gsettings set org.gnome.desktop.wm.preferences \
    focus-mode 'sloppy'                         #settings

set si cin                                        #.vimrc
set ts=4 sw=4 noet                                #.vimrc
set cb=unnamedplus                                #.vimrc

(global-set-key (kbd "C-x <next>") 'other-window) #.emacs
(global-set-key (kbd "C-x <prior>") \             #.emacs
    'previous-multiframe-window)                  #.emacs
(global-set-key (kbd "C-M-z") 'ansi-term)         #.emacs
                                                  #.emacs
(global-linum-mode 1)                             #.emacs
(column-number-mode 1)                            #.emacs
(show-paren-mode 1)                               #.emacs
(setq-default indent-tabs-mode nil)               #.emacs


valgrind --vgdb-error=0 ./a <inp &              #valgrind
gdb a                                           #valgrind
target remote | vgdb                            #valgrind

#!end_codebook
