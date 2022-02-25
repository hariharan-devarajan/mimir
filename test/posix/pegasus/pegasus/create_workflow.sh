#!/bin/bash

python ../workflow.py --mimir_bin /home/hariharan/CLionProjects/mimir/build/test/posix/ --pfs /home/hariharan/temp/mimir/pfs --shm /home/hariharan/temp/mimir/shm

pegasus-plan --dir work --dax workflow.yml --output-site local --cluster whole
