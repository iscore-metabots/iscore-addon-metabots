#include "iscore_addon_metabots.hpp"
#include <iscore/plugins/customfactory/FactorySetup.hpp>
#include <iscore_addon_metabots_commands_files.hpp>

std::pair<const CommandParentFactoryKey, CommandGeneratorMap> iscore_addon_metabots::make_commands()
{/*
    std::pair<const CommandParentFactoryKey, CommandGeneratorMap> cmds{Audio::CommandFactoryName(), CommandGeneratorMap{}};

    using Types = TypeList<
#include <iscore_addon_metabots_commands.hpp>
      >;
    for_each_type<Types>(iscore::commands::FactoryInserter{cmds.second});

    return cmds;
    */
    return {};
}

std::vector<std::unique_ptr<iscore::FactoryInterfaceBase>> iscore_addon_metabots::factories(
        const iscore::ApplicationContext& ctx,
        const iscore::AbstractFactoryKey& key) const
{
    return instantiate_factories<iscore::ApplicationContext, TL<>>(ctx, key);
}

iscore_addon_metabots::iscore_addon_metabots()
{

}

iscore_addon_metabots::~iscore_addon_metabots()
{

}

iscore::Version iscore_addon_metabots::version() const
{
    return iscore::Version{1};
}

UuidKey<iscore::Plugin> iscore_addon_metabots::key() const
{
    return_uuid("ed87a509-7319-4303-8cf7-3bba849458cf");
}
