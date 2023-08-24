# This file is used to enable the "Used With Nanite" property of every material

import unreal

classNames = unreal.Array(unreal.Name)
classNames.append(unreal.Name("Material"))

packagePaths = unreal.Array(unreal.Name)
packagePaths.append(unreal.Name('/Game'))

myFilter = unreal.ARFilter(class_names=classNames, package_paths=packagePaths, recursive_paths=True)
asset_reg = unreal.AssetRegistryHelpers.get_asset_registry()

assetsData = asset_reg.get_assets(myFilter)

for assetData in assetsData:
    uobject = assetData.get_asset()
    uobject.set_editor_property("bUsedWithNanite", True)
