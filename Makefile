OUTPUTDIR=out
OMLOBJDIR=obj/oml
OMLSRCDIR=src/oml
OMLOBJS=$(patsubst $(OMLSRCDIR)/%.c,$(OMLOBJDIR)/%.o,$(wildcard $(OMLSRCDIR)/*.c))
OMLCFLAGS=-Wall -O2 -fopenmp -msse2 -Iinclude
LIBOML=$(OUTPUTDIR)/liboml.a

all: $(LIBOML)

clean:
	$(RM) $(OMLOBJS)

$(OMLOBJS): | $(OMLOBJDIR)

$(OMLOBJDIR):
	mkdir -p $(OMLOBJDIR)

$(OMLOBJDIR)/%.o : $(OMLSRCDIR)/%.c
	$(CC) $(OMLCFLAGS) -c -o $@ $<

$(OUTPUTDIR):
	mkdir -p $(OUTPUTDIR)
	
$(LIBOML): $(OMLOBJS) | $(OUTPUTDIR)
	$(AR) r $@ $<
