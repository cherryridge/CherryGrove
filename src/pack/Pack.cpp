#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/uuid.hpp>

#include "../debug/Fatal.hpp"
#include "../debug/Logger.hpp"
#include "../settings/Settings.hpp"
#include "../umi/frontend/lua/Luau.hpp"
#include "../umi/frontend/js/V8.hpp"
#include "../umi/frontend/wasm/Wasmtime.hpp"
#include "base.hpp"
#include "Pack.hpp"
#include "packFetcher.hpp"

namespace Pack {
    typedef uint64_t u64;
    using std::filesystem::current_path, std::vector, std::string, boost::uuids::uuid, boost::unordered_flat_map;

    static unordered_flat_map<uuid, pair<Latest, path>> registry;

    void init() noexcept {
        if (!PHYSFS_init(current_path().string().c_str())) {
            lerr << "[Pack] Failed to initialize PhysFS: " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) << endl;
            Fatal::exit(Fatal::FILESYSTEM_CANNOT_INIT_PHYSFS);
        }
        //Just use additional packs/roots instead of symlinking all your packs to `/packs`. It's much safer.
        PHYSFS_permitSymbolicLinks(false);

        UmiJSON::init();
        UmiV8::init();
        UmiLuau::init();
        UmiWasmtime::init();

        const Settings::Settings& settings = Settings::getData();
        vector<string> potentialPackRoots = settings.packs;
        potentialPackRoots.push_back("packs");
        getPacksFromPackRoots(potentialPackRoots, registry);
        for (u64 i = 0; i < packSettings.additionalPacks.size(); i++) {
            PackDesc desc;
            if (getSinglePack(packSettings.additionalPacks[i], desc)) registry.emplace(desc.id, PackDescWithPath{
                .desc = move(desc),
                ._path = packSettings.additionalPacks[i]
            });
            else lerr << "[Pack] Failed to load pack: " << packSettings.additionalPacks[i] << endl;
        }
        lout << "[Pack] Found " << registry.size() << " valid packs." << endl;
    }

    void shutdown() noexcept {
        UmiWasmtime::shutdown();
        UmiLuau::shutdown();
        UmiV8::shutdown();
        UmiJSON::shutdown();
        PHYSFS_deinit();
    }
}