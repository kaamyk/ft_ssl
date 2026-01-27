.SILENT:
	
NAME	=	ft_ssl

SRCDIR	=	src
SRCS	=	$(SRCDIR)/main.c		$(SRCDIR)/parse.c	$(SRCDIR)/sha256.c	$(SRCDIR)/md5.c	\
			$(SRCDIR)/routine.c 	$(SRCDIR)/utils.c	$(SRCDIR)/error.c	$(SRCDIR)/ft_strjoin.c \
			$(SRCDIR)/stdin.c		$(SRCDIR)/digest.c	$(SRCDIR)/cipher.c	$(SRCDIR)/display.c \
			$(SRCDIR)/base64.c
			
DEPDIR	=	.dep
DEPS	=	$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.d, $(SRCS))

OBJDIR	=	.obj
OBJS	=	$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

CC		=	gcc
C_FLAGS	=	-Wall -Wextra -Werror -g -O2

$(OBJDIR)/%.o :	$(SRCDIR)/%.c
	echo -n "Compiling " $< " ... "
	$(CC) $(C_FLAGS) -c $< -o $@
	$(CC) $(C_FLAGS) -MM $< | sed 's,\($*\)\.o[ :]*,$(DEPDIR)/\1.o $@ : ,g' > $(DEPDIR)/$*.d
	echo -e '\x1b[32m>>> OK <<<\x1b[37m'	

all :		$(OBJDIR) $(DEPDIR) $(OBJS) $(NAME)

$(NAME):	$(OBJDIR) $(DEPDIR) $(OBJS)
	echo -e '\033[0;34mObjects compilation: \x1b[32m>>> OK <<<\x1b[37m'
	echo ""
	echo -n "Compiling " $(NAME) " ... "
	$(CC) $(C_FLAGS) $(OBJS) -lm -o $(NAME)
	echo -e '\x1b[32m>>> OK <<<\x1b[37m'
	echo -en '\033[0;34mExecutable '
	echo -n  $(NAME)
	echo -e  ' compiled: \x1b[32m>>> OK <<<\x1b[37m'
	echo ""

$(OBJDIR):
	mkdir -v $(OBJDIR)

$(DEPDIR):
	mkdir -v $(DEPDIR)

clean:
	rm -rfv $(OBJDIR) $(DEPDIR)
	echo -e '\033[0;34mObject and Dependencies files removed: \x1b[32m>>> OK <<<\x1b[37m'

fclean: clean
	rm -fv $(NAME) $(NAME).d
	echo -e '\033[0;34mExecutable file removed: \x1b[32m>>> OK <<<\x1b[37m'

-include $(DEPS)

re: fclean all

.PHONY: all clean fclean