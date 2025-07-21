# Make database script Unix version.
#
# 2021.4.19 / sutezo
#
PYTHON := python3
THISYEAR := $(shell date +%Y)
NEWDB := Newest
DB = 2014 2015 2017 $(NEWDB)
SHEETS := fare2 rule86 rule70_new rule69 clinfar2014 lines2014 clinfar2015 clinfarNewest lines2015 lines2017 linesNewest brt

DBFILES := $(addprefix jrdb, $(patsubst %, %.db, $(DB)))

IOS_DBDIR := ../app/Farert.ios/Farert/
WIN_DBDIR := ../app/win_mfc/fjr_mfc/alps_mfc/res/
DB_ANDROID := ../app/Farert.android/app/src/main/assets/routeDB/jrdb.dat
DB_IOS := $(addprefix $(IOS_DBDIR), $(DBFILES))
DB_WIN := $(addprefix $(WIN_DBDIR), $(DBFILES))

ifdef $(farertDB)
	$(error "Could defined 'farertDB' variable")
endif

define COPY
$(1) : $(2)
	cp -f $(2) $(1)
endef

define MKDB
$(1) : $(patsubst %.db, %.txt, $(1))
	echo ""
	echo "===scripts/jr_db_reg.py $(patsubst %.db, %.txt, $(1)) ==="
	echo ""
	$(PYTHON) scripts/jr_db_reg.py $(patsubst %.db, %.txt, $(1)) \
		$(patsubst $(NEWDB),$(THISYEAR),$(patsubst jrdb%.db, %, $(1)))
	$(PYTHON) scripts/node_list.py $(1) > scripts/node_list.txt
endef

.PHONY : clean download

all : $(patsubst %.db, %.txt, $(DBFILES)) $(DBFILES) $(DB_IOS) $(DB_WIN) $(DB_ANDROID)

# make .txt from google spreadsheet
$(patsubst %.db, %.txt, $(DBFILES)): $(patsubst %, %.tmp, $(SHEETS))

$(patsubst %, %.tmp, $(SHEETS)) : download

# make db
$(foreach OBJ, $(DBFILES), $(eval $(call MKDB, $(OBJ))))

# make for Android
$(DB_ANDROID) : $(DBFILES)
	zip $(DB_ANDROID) $(DBFILES)

# iOS (DB_IOS)
$(foreach OBJ, $(DBFILES), $(eval $(call COPY, $(addprefix $(IOS_DBDIR), $(OBJ)), $(OBJ))))

# Windows (DB_WIN)
$(foreach OBJ, $(DBFILES), $(eval $(call COPY, $(addprefix $(WIN_DBDIR), $(OBJ)), $(OBJ))))

update : clean all

download :
	./down_spr_as_text.sh $(patsubst %.db,%.txt,$(DBFILES))

clean:
	rm -f *.tmp *.db
