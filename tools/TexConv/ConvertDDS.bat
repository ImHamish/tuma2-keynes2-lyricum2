for %%f in (*.png) do (
  .\texconv.exe -f DXT5 -srgbi -srgbo -m 1 -ft dds "%%~f"
)
