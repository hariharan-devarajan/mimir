#!/usr/bin/env python3
import os
import logging
from pathlib import Path
from argparse import ArgumentParser
import shutil
from pathlib import Path

logging.basicConfig(level=logging.DEBUG)

# --- Import Pegasus API ------------------------------------------------------
from Pegasus.api import *


class MimirWorkflow:
    wf = None
    sc = None
    tc = None
    rc = None
    props = None

    dagfile = None
    wf_name = None
    wf_dir = None
    pmc = None
    intercept = None

    mimir_bin = None
    pfs = None
    shm = None

    # --- Init ----------------------------------------------------------------
    def __init__(self,
                 mimir_bin,
                 pfs,
                 shm,
                 pmc=True,
                 intercept=False):
        self.mimir_bin = mimir_bin
        self.pfs = pfs
        self.shm = shm
        self.dagfile = "workflow.yml"
        self.wf_name = "pegasus_io_test"
        self.wf_dir = str(Path(__file__).parent.resolve())
        self.pmc = pmc
        self.intercept = intercept

    # --- Write files in directory --------------------------------------------
    def write(self):
        if not self.sc is None:
            self.sc.write()
        self.props.write()
        self.tc.write()
        self.rc.write()
        self.wf.write()

    # --- Configuration (Pegasus Properties) ----------------------------------
    def create_pegasus_properties(self):
        self.props = Properties()
        self.props["pegasus.register"] = "false"
        self.props["pegasus.integrity.checking"] = "none"
        if self.pmc:
            self.props["pegasus.job.aggregator"] = "mpiexec"
            self.props["pegasus.data.configuration"] = "sharedfs"
        return

    # --- Site Catalog --------------------------------------------------------
    def create_sites_catalog(self, exec_site_name="condorpool"):
        self.sc = SiteCatalog()

        shared_scratch_dir = os.path.join(self.wf_dir, "scratch")
        local_storage_dir = os.path.join(self.wf_dir, "output")

        local = Site("local").add_directories(
            Directory(Directory.SHARED_SCRATCH, shared_scratch_dir).add_file_servers(
                FileServer("file://" + shared_scratch_dir, Operation.ALL)
            ),
            Directory(Directory.LOCAL_STORAGE, local_storage_dir).add_file_servers(
                FileServer("file://" + local_storage_dir, Operation.ALL)
            ),
        )

        executable_path = Path(shutil.which('pegasus-status')).parent.parent.absolute();

        exec_site = (
            Site(exec_site_name)
                .add_pegasus_profile(style="condor")
                .add_condor_profile(universe="vanilla")
                .add_profiles(Namespace.PEGASUS, key="data.configuration", value="sharedfs")
                .add_profiles(Namespace.PEGASUS, key="auxillary.local", value="true")
                .add_profiles(Namespace.ENV, key="PEGASUS_HOME", value=f"{executable_path}")
                .add_directories(
                Directory(Directory.SHARED_SCRATCH, shared_scratch_dir).add_file_servers(
                    FileServer("file://" + shared_scratch_dir, Operation.ALL)
                ),
                Directory(Directory.LOCAL_STORAGE, local_storage_dir).add_file_servers(
                    FileServer("file://" + local_storage_dir, Operation.ALL)
                ),
            )
        )

        self.sc.add_sites(local, exec_site)

    # --- Transformation Catalog (Executables and Containers) -----------------
    def create_transformation_catalog(self, exec_site_name="condorpool"):
        self.tc = TransformationCatalog()

        path = self.mimir_bin
        filename = os.path.join(path, "pegasus")
        raw = Transformation(
            "pegasus_raw", site=exec_site_name, pfn=filename, is_stageable=False,
        )
        write = Transformation(
            "pegasus_write", site=exec_site_name, pfn=filename, is_stageable=False,
        )
        read = Transformation(
            "pegasus_read", site=exec_site_name, pfn=filename, is_stageable=False,
        )

        self.tc.add_transformations(raw, write, read)

    # --- Replica Catalog ------------------------------------------------------
    def create_replica_catalog(self):
        self.rc = ReplicaCatalog()

    # --- Create Workflow -----------------------------------------------------
    def create_workflow(self):
        self.wf = Workflow(self.wf_name, infer_dependencies=True)
        file_data = File("test.dat")
        ld_preload = ""
        if self.intercept:
            if "ATHENA_LIB_PATH" not in os.environ:
                raise Exception('ATHENA_LIB_PATH not set. Needs to point to libathena.so.')
            ld_preload = os.environ["ATHENA_LIB_PATH"]

        # RAW usecase no job dependency.
        raw = (
            Job("pegasus_raw")
                .add_args("--durations", "yes", "--reporter", "compact",
                          "--pfs", self.pfs, "--shm", self.shm, "--filename", "raw.dat", "[operation=raw]")
                .add_profiles(Namespace.ENV, key="LD_PRELOAD", value=f"{ld_preload}")
        )
        # Read job depends on Write job.
        write = (
            Job("pegasus_write")
                .add_args("--durations", "yes", "--reporter", "compact",
                          "--pfs", self.pfs, "--shm", self.shm, "--filename", "write_job.dat", "[operation=write]")
                .add_outputs(file_data, stage_out=False, register_replica=False)
                .add_profiles(Namespace.ENV, key="LD_PRELOAD", value=f"{ld_preload}")
        )
        read = (
            Job("pegasus_read")
                .add_args("--durations", "yes", "--reporter", "compact",
                          "--pfs", self.pfs, "--shm", self.shm, "--filename", "write_job.dat", "[operation=read]")
                .add_inputs(file_data)
                .add_profiles(Namespace.ENV, key="LD_PRELOAD", value=f"{ld_preload}")
        )
        self.wf.add_jobs(raw, write, read)


if __name__ == "__main__":
    parser = ArgumentParser(description="Pegasus Write + Read Workflow")

    parser.add_argument(
        "--pfs",
        metavar="STR",
        type=str,
        default="${PWD}",
        help="Path for PFS",
    )
    parser.add_argument(
        "--shm",
        metavar="STR",
        type=str,
        default="/dev/shm",
        help="Path for SHM",
    )
    parser.add_argument(
        "--mimir_bin",
        metavar="STR",
        type=str,
        default="${PWD}",
        help="Binary directory for Mimir tests",
    )
    parser.add_argument('--intercept', action='store_true', dest='intercept', help='Intercept using Athena')

    args = parser.parse_args()

    workflow = MimirWorkflow(args.mimir_bin, args.pfs, args.shm, intercept=args.intercept)

    print("Creating execution sites...")
    workflow.create_sites_catalog()

    print("Creating workflow properties...")
    workflow.create_pegasus_properties()

    print("Creating transformation catalog...")
    workflow.create_transformation_catalog()

    print("Creating replica catalog...")
    workflow.create_replica_catalog()

    print("Creating split workflow dag...")
    workflow.create_workflow()

    workflow.write()
