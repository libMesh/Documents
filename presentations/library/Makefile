
default: all
	@true # override later % rule

dirlist := \
arl \
arl2 \
cilamce_shortcourse_2014 \
erdctalk \
general_field_transfer \
gridclasstalk \
LaRC_overview_2014 \
libmeshflows1 \
libmeshslides \
nasa_ES_2013 \
oil_gas_hpc_2016 \
prace_shortcourse_2013 \
psaap2_talk_2013 \
sandiatalk \
ssd_talk \
ssd_talk_2016 \
ub_workshop_2015 \
vicksburg


clean:
	@echo make clean
	@for dir in $(dirlist); do \
	  if [ -d "$${dir}"/roys_build_dir ]; then \
	    if [ -f "$${dir}"/roys_build_dir/Makefile ]; then \
	      if [ -f "$${dir}"/roys_build_dir/clean_failed ]; then \
                echo "Skipping broken clean in $${dir}"; \
              else \
                echo "Running clean in $${dir}/roys_build_dir/"; \
	        $(MAKE) -C "$${dir}"/roys_build_dir/ clean || touch "$${dir}"/roys_build_dir/clean_failed; \
	      fi; \
	      if [ -f "$${dir}"/roys_build_dir/maintainer_clean_failed ]; then \
                echo "Skipping broken maintainer-clean in $${dir}"; \
              else \
                echo "Running maintainer-clean in $${dir}/roys_build_dir/"; \
	        $(MAKE) -C "$${dir}"/roys_build_dir/ maintainer-clean || touch "$${dir}"/roys_build_dir/maintainer_clean_failed; \
	      fi; \
	      if [ ! -f "$${dir}"/roys_build_dir/maintainer_clean_failed ]; then \
	        rm -rf "$${dir}"/roys_build_dir; \
	      fi; \
	    fi; \
	  elif [ -f "$${dir}"/Makefile ]; then \
	    if [ -f "$${dir}"/bootstrap ]; then \
	      if [ -f "$${dir}"/maintainer_clean_failed ]; then \
                echo "Skipping broken maintainer-clean in $${dir}"; \
              else \
                echo "Running maintainer-clean in $${dir}"; \
	        $(MAKE) -C "$${dir}" maintainer-clean || touch "$${dir}"/maintainer_clean_failed; \
	      fi; \
            else \
	      if [ -f "$${dir}"/clean_failed ]; then \
                echo "Skipping broken clean in $${dir}"; \
              else \
                echo "Running clean in $${dir}"; \
	        $(MAKE) -C "$${dir}" clean || touch "$${dir}"/clean_failed; \
	      fi; \
	    fi; \
	  fi; \
	done

%:
	@echo make $@
	@for dir in $(dirlist); do \
	  if [ -f "$${dir}"/bootstrap ]; then \
	    if [ -f "$${dir}"/bootstrap_failed ]; then \
              echo "Skipping broken bootstrap in $${dir}"; \
            else \
              echo "Running bootstrap in $${dir}"; \
	      (cd "$${dir}" && ./bootstrap || touch bootstrap_failed); \
	      mkdir -p "$${dir}"/roys_build_dir; \
	    fi; \
	  fi; \
	  if [ -d "$${dir}"/roys_build_dir ]; then \
	    if [ -f "$${dir}"/configure ]; then \
	      if [ -f "$${dir}"/roys_build_dir/configure_failed ]; then \
                echo "Skipping broken configure in $${dir}/roys_build_dir"; \
              else \
                 echo "Running configure in $${dir}/roys_build_dir"; \
	         (cd "$${dir}"/roys_build_dir && ../configure || touch configure_failed); \
	      fi; \
	    fi; \
	    if [ -f "$${dir}"/roys_build_dir/Makefile ]; then \
	      if [ -f "$${dir}"/roys_build_dir/configure_failed ]; then \
                echo "Skipping broken make $@ in $${dir}"; \
              else \
                echo "Running make $@ in $${dir}"; \
	        $(MAKE) -C "$${dir}"/roys_build_dir $@ || touch "$${dir}"/roys_build_dir/make_$@_failed; \
	      fi; \
	    fi; \
	  else \
            if [ -f "$${dir}"/configure ]; then \
	      if [ -f "$${dir}"/configure_failed ]; then \
                echo "Skipping broken configure in $${dir}"; \
              else \
                 echo "Running configure in $${dir}"; \
	         (cd "$${dir}" && ./configure || touch configure_failed); \
	      fi; \
	    fi; \
	    if [ -f "$${dir}"/Makefile ]; then \
	      if [ -f "$${dir}"/configure_failed ]; then \
                echo "Skipping broken make $@ in $${dir}"; \
              else \
                echo "Running make $@ in $${dir}"; \
	        $(MAKE) -C "$${dir}" $@ || touch "$${dir}"/make_$@_failed; \
	      fi; \
	    fi; \
	  fi; \
	done
