from __future__ import annotations

import argparse
import json
import os
import platform
import shutil
import subprocess
import sys
import textwrap
from pathlib import Path


class print_color:
    if sys.stdout.isatty() and os.environ.get("TERM") != "dumb":
        RESET   = "\033[0m"
        BOLD    = "\033[1m"
        DIM     = "\033[2m"
        RED     = "\033[31m"
        GREEN   = "\033[32m"
        YELLOW  = "\033[33m"
        BLUE    = "\033[34m"
        CYAN    = "\033[36m"
    else:
        RESET = BOLD = DIM = RED = GREEN = YELLOW = BLUE = CYAN = ""


C = print_color


def step(n: int, msg: str) -> None:
    print(f"\n{C.BOLD}{C.CYAN}[{n}] {msg}{C.RESET}")


def ok(msg: str) -> None:
    print(f"  {C.GREEN}OK{C.RESET}  {msg}")


def warn(msg: str) -> None:
    print(f"  {C.YELLOW}WARN{C.RESET}  {msg}")


def err(msg: str) -> None:
    print(f"  {C.RED}ERR{C.RESET}  {msg}")


def info(msg: str) -> None:
    print(f"  {C.DIM}..{C.RESET}    {msg}")


def run(cmd: list[str], *, check: bool = True, capture: bool = False,
        cwd: str | None = None) -> subprocess.CompletedProcess:
    info(f"$ {' '.join(cmd)}")
    return subprocess.run(
        cmd,
        check=check,
        cwd=cwd,
        text=True,
        capture_output=capture,
    )


def which(exe: str) -> str | None:
    return shutil.which(exe)


DEFAULT_UE_ROOTS = [
    r"C:\Program Files\Epic Games\UE_5.4",
    r"D:\Program Files\Epic Games\UE_5.4",
    r"C:\Epic Games\UE_5.4",
]


def find_ue_root(override: str | None) -> Path:
    if override:
        p = Path(override)
        if not p.exists():
            raise SystemExit(f"yo this --ue-root path doesnt exist: {p}")
        return p
    for candidate in DEFAULT_UE_ROOTS:
        if Path(candidate).exists():
            return Path(candidate)
    raise SystemExit(
        "Yo I cant find it manually where u installed ur Unreal Engine 5.4? "
        "Pass it like this:  --ue-root \"C:\\path\\to\\UE_5.4\"."
    )


DOTNET6_SDK_URL = (
    "https://builds.dotnet.microsoft.com/dotnet/Sdk/6.0.428/"
    "dotnet-sdk-6.0.428-win-x64.exe"
)
DOTNET6_SDK_FILENAME = "dotnet-sdk-6.0.428-win-x64.exe"


def download_file(url: str, dest: Path) -> None:
    import urllib.request

    info(f"alright pulling this down: {url}")
    info(f"        ->  {dest}")

    def _hook(block_num: int, block_size: int, total_size: int) -> None:
        if total_size <= 0:
            return
        downloaded = block_num * block_size
        pct = min(100, downloaded * 100 // total_size)
        mb_done = downloaded / (1024 * 1024)
        mb_total = total_size / (1024 * 1024)
        sys.stdout.write(
            f"\r        {pct:3d}%  {mb_done:6.1f} / {mb_total:6.1f} MB"
        )
        sys.stdout.flush()

    dest.parent.mkdir(parents=True, exist_ok=True)
    urllib.request.urlretrieve(url, dest, _hook)
    sys.stdout.write("\n")
    sys.stdout.flush()


def install_dotnet6_manual() -> None:
    dest = Path(os.environ.get("TEMP", ".")) / DOTNET6_SDK_FILENAME

    if dest.exists() and dest.stat().st_size > 50_000_000:
        ok(f"installer is already sitting here so skipping the download: {dest}")
    else:
        try:
            download_file(DOTNET6_SDK_URL, dest)
        except Exception as e:
            err(f"download tanked on me: {e}")
            info(f"u can grab it manually here: {DOTNET6_SDK_URL}")
            raise SystemExit(1)
        ok(f"got it: {dest}")

    info("Yo I am launching installer just install it ITS OUTDATED BUT its ue5.4 anyways - this bich got z-up convention whatelse can u expect")
    info("then return here this script is paused until the installer is finished")

    try:
        completed = subprocess.run([str(dest)], check=False)
    except OSError as e:
        err(f"couldnt fire up the installer: {e}")
        info(f"just double click it yourself: {dest}")
        raise SystemExit(1)

    if completed.returncode == 0:
        ok("installer says it's done, nice")
    elif completed.returncode == 1602:
        warn("Why you cancelded it? theres no other way around u know that right?")
    else:
        warn(f"exited code: if its an error lmk {completed.returncode}")


def check_dotnet(install_dotnet6: bool) -> None:
    if which("dotnet") is None:
        err("yo there's no 'dotnet' on PATH at all")
        if install_dotnet6:
            info("ok ill grab the .NET 6 SDK installer and launch it for u")
            install_dotnet6_manual()
            info("close this terminal and open a fresh one so PATH refreshes")
            info("then run this script again")
            raise SystemExit(0)
        info(f"grab .NET 6 SDK from here: {DOTNET6_SDK_URL}")
        info("or just re-run the script with --install-dotnet6 and ill handle it")
        raise SystemExit(1)

    sdks = run(["dotnet", "--list-sdks"], capture=True).stdout.strip().splitlines()
    info(f"found {len(sdks)} SDKs already on ur machine")
    for s in sdks:
        info(f"  {s}")

    has_net6 = any(line.startswith("6.") for line in sdks)
    if has_net6:
        ok("ur all good, .NET 6 SDK is here.")
        return

    warn("no .NET 6 SDK on ur machine btw")
    warn("heres the deal: UE 5.4's UnrealBuildTool wants net6.0 specifically.")
    warn("yeah newer SDKs CAN technically build it but installing 6.0 is the cleanest")
    warn("fix - its just the way of unreal engine ig")

    if install_dotnet6:
        install_dotnet6_manual()
        sdks = run(["dotnet", "--list-sdks"], capture=True, check=False).stdout.splitlines()
        if any(line.startswith("6.") for line in sdks):
            ok("yep .NET 6 SDK is showing up now, we good")
        else:
            warn("hmm cant see .NET 6 SDK in this shell yet")
            info("open a fresh terminal so PATH refreshes, then run me again")
    else:
        info("re-run me with --install-dotnet6 and ill download + launch the installer")
        info(f"or grab it manually urself: {DOTNET6_SDK_URL}")


def ensure_nuget_org_source() -> None:
    result = run(["dotnet", "nuget", "list", "source"], capture=True, check=False)
    output = (result.stdout or "") + (result.stderr or "")
    if "api.nuget.org/v3/index.json" in output:
        ok("nuget.org is already a registered source, we chillin")
        return

    warn("nuget.org is NOT registered! as a NuGet source Adding it now.")
    run([
        "dotnet", "nuget", "add", "source",
        "https://api.nuget.org/v3/index.json",
        "-n", "nuget.org",
    ])
    ok("done, nuget.org is now in ur global sources")


def write_local_nuget_config(ubt_dir: Path) -> None:
    cfg_path = ubt_dir / "NuGet.Config"
    contents = textwrap.dedent("""\
        <?xml version="1.0" encoding="utf-8"?>
        <configuration>
          <packageSources>
            <clear />
            <add key="nuget.org" value="https://api.nuget.org/v3/index.json" protocolVersion="3" />
          </packageSources>
        </configuration>
    """)
    if cfg_path.exists():
        existing = cfg_path.read_text(encoding="utf-8", errors="ignore")
        if "api.nuget.org" in existing:
            ok(f"local NuGet.Config already points at nuget.org, skipping: {cfg_path}")
            return
        backup = cfg_path.with_suffix(".Config.bak")
        info(f"backing up the old NuGet.Config first -> {backup.name}")
        cfg_path.replace(backup)
    cfg_path.write_text(contents, encoding="utf-8")
    ok(f"dropped a fresh NuGet.Config at {cfg_path}")


def clear_nuget_http_cache() -> None:
    run(["dotnet", "nuget", "locals", "http-cache", "--clear"], check=False)
    ok("nuked the NuGet HTTP cache")


def restore_ubt(ubt_csproj: Path) -> bool:
    info(f"restoring this thing: {ubt_csproj}")
    proc = run(
        ["dotnet", "restore", str(ubt_csproj), "--force", "--no-cache"],
        check=False,
    )
    if proc.returncode == 0:
        ok("dotnet restore worked, UBT is happy now")
        return True
    err("dotnet restore died on me. scroll up and read the output - the reason is in there")
    return False


def strip_uproject_plugin(uproject: Path, plugin_name: str) -> bool:
    if not uproject.exists():
        err(f"cant find ur .uproject: {uproject}")
        return False

    raw = uproject.read_text(encoding="utf-8-sig")
    try:
        data = json.loads(raw)
    except json.JSONDecodeError as e:
        err(f"ur {uproject.name} is not valid JSON: {e}")
        info("basically the file is already broken before i touched it. open it")
        info("in any text editor, make sure its real JSON, then come back")
        return False

    plugins = data.get("Plugins")
    if not isinstance(plugins, list) or not plugins:
        ok(f"no 'Plugins' array in {uproject.name}, nothing to do here")
        return False

    keep = [p for p in plugins
            if not (isinstance(p, dict) and p.get("Name") == plugin_name)]
    removed = len(plugins) - len(keep)
    if removed == 0:
        ok(f"'{plugin_name}' isnt in {uproject.name} anyway, skipping")
        return False

    backup = uproject.with_suffix(uproject.suffix + ".bak")
    info(f"making a backup first just in case -> {backup.name}")
    shutil.copy2(uproject, backup)

    if keep:
        data["Plugins"] = keep
    else:
        data.pop("Plugins", None)

    uproject.write_text(
        json.dumps(data, indent=2) + "\n",
        encoding="utf-8",
    )
    ok(f"yeeted {removed} '{plugin_name}' entry out of {uproject.name}")
    return True


def clean_project_intermediates(uproject: Path) -> None:
    project_dir = uproject.parent
    targets = ["Binaries", "Intermediate", ".vs", "DerivedDataCache"]
    deleted_any = False
    for name in targets:
        p = project_dir / name
        if not p.exists():
            continue
        info(f"deleting {p}")
        try:
            shutil.rmtree(p, ignore_errors=False)
            deleted_any = True
        except OSError as e:
            warn(f"couldnt delete {p}: {e}")
            warn("close VS and the Unreal Editor first, then run me again")
    if not deleted_any:
        ok("no stale build folders, ur tree was already clean")
    else:
        ok("all the junk build folders are gone")


def regenerate_project_files(ue_root: Path, uproject: Path) -> None:
    if not uproject.exists():
        err(f"cant find ur .uproject: {uproject}")
        return
    bat = ue_root / "Engine" / "Build" / "BatchFiles" / "Build.bat"
    if not bat.exists():
        err(f"Build.bat is not at {bat} - ur UE install might be cooked")
        return
    info(f"regenerating project files for {uproject.name}, hold on")
    run([
        str(bat),
        "-projectfiles",
        f"-project={uproject}",
        "-game", "-rocket", "-progress",
    ], check=False)
    ok("project files regenerated. now reopen the .sln in Visual Studio")

def main() -> int:
    parser = argparse.ArgumentParser(
        description="fixes the C++ build env for Unreal Engine 5.4 on Windows so u can actually compile.",
    )
    parser.add_argument("--ue-root", help="path to UE_5.4 root (ill autodetect if u skip this)")
    parser.add_argument("--uproject", help="optional .uproject so i regen its VS files too")
    parser.add_argument("--install-dotnet6", action="store_true",
                        help="if .NET 6 is missing, ill download the 6.0.428 SDK installer "
                             "and launch it for u (u still gotta click thru tho)")
    parser.add_argument("--skip-restore", action="store_true",
                        help="fix the config but skip the dotnet restore step")
    parser.add_argument("--no-strip-vs-plugin", action="store_true",
                        help="dont remove the VisualStudioTools plugin from ur .uproject "
                             "(by default i yeet it)")
    parser.add_argument("--no-clean", action="store_true",
                        help="dont delete Binaries/Intermediate/.vs/DDC before regenerating "
                             "project files")
    args = parser.parse_args()

    print(f"{C.BOLD}Unreal Engine 5.4: C++ build environment fixer{C.RESET}")

    step(1, "checking ur on windows")
    if platform.system() != "Windows":
        err("yo this script is windows only. ur on: " + platform.system())
        return 1
    ok(f"Windows {platform.release()} ({platform.machine()})")

    step(2, "hunting for ur Unreal Engine 5.4 install")
    ue_root = find_ue_root(args.ue_root)
    ok(f"found UE here: {ue_root}")
    ubt_dir = ue_root / "Engine" / "Source" / "Programs" / "UnrealBuildTool"
    ubt_csproj = ubt_dir / "UnrealBuildTool.csproj"
    if not ubt_csproj.exists():
        err(f"UnrealBuildTool.csproj is missing from {ubt_csproj} - ur UE install is busted")
        return 1
    ok(f"UBT project: {ubt_csproj}")

    step(3, "checking .NET SDK situation")
    try:
        check_dotnet(args.install_dotnet6)
    except SystemExit as e:
        return int(e.code) if isinstance(e.code, int) else 1

    step(4, "making sure nuget.org is a registered source")
    ensure_nuget_org_source()

    step(5, "dropping a project-local NuGet.Config")
    write_local_nuget_config(ubt_dir)

    step(6, "wiping the NuGet HTTP cache")
    clear_nuget_http_cache()

    if args.skip_restore:
        step(7, "skipping dotnet restore (--skip-restore was passed)")
    else:
        step(7, "running dotnet restore on UBT")
        if not restore_ubt(ubt_csproj):
            return 1

    if args.uproject:
        uproject = Path(args.uproject).expanduser().resolve()
        if not uproject.exists():
            err(f"cant find ur .uproject: {uproject}")
            return 1
        info(f"using project file: {uproject}")
        if args.no_strip_vs_plugin:
            step(8, "skipping VisualStudioTools plugin strip (--no-strip-vs-plugin was passed)")
        else:
            step(8, "yeeting 'VisualStudioTools' plugin out of ur .uproject")
            strip_uproject_plugin(uproject, "VisualStudioTools")
        if args.no_clean:
            step(9, "skipping clean of project intermediates (--no-clean was passed)")
        else:
            step(9, "cleaning out the junk build folders")
            clean_project_intermediates(uproject)
        step(10, "regenerating Visual Studio project files")
        regenerate_project_files(ue_root, uproject)

    print(f"\n{C.BOLD}{C.GREEN}done.{C.RESET}")
    print(textwrap.dedent(f"""
        alright ur up next:
          1. open ur project's from unreal engine should build and then refresh vs studio then open it
          2. it can cry about old outdated packages but we cant help just update it, it should fail but get the cache going
          3. u can see error indicator on the code just wait a little if it still shows then go to the editor setting on unreal engine then go to source code and change ide for 22 or 26 refresh it should be fine then
    """))
    return 0


if __name__ == "__main__":
    sys.exit(main())
