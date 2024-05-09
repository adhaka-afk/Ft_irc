############################### Files and directories ###############################
# Source files
SRCS	 = main Server Client Channel Away Invite Join Kick List Mode Names Nick \
		   Notice Part Pass Ping Privmsg Quit Topic User Logger

# Directories
SRC_DIR = src
OBJ_DIR = obj

##############################        Objects        ################################
OBJS	+= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(SRCS)))

##############################  Config declarations  #################################
NAME	= ircserv
INCL	= inc
OPTS	=

VPATH	+= %.cpp $(SRC_DIR) $(SRC_DIR)/commands

CC		= c++
FLAGS	= -g -O0 -Wall -Wextra -Werror -std=c++98
RM		= rm -rf

all:		${NAME}

${NAME}:	${OBJS}
			@echo "    ${NAME}"
			@${CC} ${FLAGS} ${OBJS} ${OPTS} -o ${NAME}

${OBJ_DIR}/%.o: %.cpp | ${OBJ_DIR}
			@echo "    $<"
			@${CC} ${FLAGS} -c $< -o $@ -I ${INCL}

${OBJ_DIR}:
			@mkdir -p ${OBJ_DIR}
			@mkdir -p fileshare

ircbot:
			@${CC} ${FLAGS} ${SRC_DIR}/Logger.cpp ${SRC_DIR}/bot/Bot.cpp -I ${INCL} -o ircbot

clean:
			${RM} ${OBJ_DIR}

fclean:		clean
			@${RM} fileshare
			${RM} ${NAME} ircbot

re:			fclean all

.PHONY:		all clean fclean re
