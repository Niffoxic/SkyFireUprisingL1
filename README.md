# SkyFireUprisingL1

fix unreal engine 5.4 build errors by running:

## what u need

- Python 3.8+ on PATH
- Unreal Engine 5.4 installed (script autodetects the usual spots)
- terminal opened **as admin** if ur UE is under `C:\Program Files\...`

## how to run

**also auto-install .NET 6 if its missing:**
```bash
python build_fix.py --install-dotnet6 --uproject "C:\path\to\YourGame.uproject"
```

**if ur UE is in a weird spot:**
```bash
python build_fix.py --ue-root "D:\Epic\UE_5.4" --uproject "C:\path\to\YourGame.uproject" --install-dotnet6
```
