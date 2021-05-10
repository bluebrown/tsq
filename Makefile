CC := gcc
CFLAGS := -pthread
SOURCES := $(shell find * -type f -name "*.c")
OBJDIR  := .objects
OBJECTS := $(addprefix $(OBJDIR)/,$(SOURCES:.c=.o))
OUT := a.out

-include $(OBJECTS:.o=.d)

all: $(OUT)

$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJECTS): $(OBJDIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	$(RM) -r $(OUT) $(OBJDIR)
