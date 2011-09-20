#pragma once
#ifndef CMDLINE_H_
#define CMDLINE_H_
//#SRCHEADER


#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <set>

//------------------------------------------------------------------------------
/// Command line parsing class.
class CmdLine
{
public:
    typedef std::vector< std::string > CommandParameters;
    typedef std::map< std::string, CommandParameters > ParsedEntries;
private:
    /// Private struct to store command entry information.
    struct CommandEntry
    {
        std::string description;
        std::string longName;
        std::string shortName;
        std::pair< size_t, size_t > numArguments;
        bool optional;
        CommandEntry( const std::string& desc, const std::string& lname,
            const std::string& sname, const std::pair< size_t, size_t >& nargs,
            bool opt ) : description( desc ), longName( lname ),
            shortName( sname ), numArguments( nargs ), optional( opt ) {}
    };
    
    typedef std::map< std::string, CommandEntry > Entries;
    typedef std::map< std::string, std::string > Synonyms;

public:
    /// Constructor. Invokes the Add() method.
    /// \param reportUnknownParameters if true an exception is thrown in case
    ///        of unknown parameters.
    /// \param helpLongName long command line parameter name for help e.g. 'help'
    /// \param helpShortName short command line parameter name for help e.g. 'help'
    /// \param longPrefix prefix to append to long command names e.g. '--'
    /// \param shortPrefix prefix to append to short command names e.g. '-'
    CmdLine( bool reportUnknownParameters = false,
             const std::string& helpLongName = "help",
             const std::string& helpShortName = "h",
             const std::string& longPrefix = "--",
             const std::string& shortPrefix = "-", 
             const std::string& freeArgKey = " ", //add command line items not in parameter lists to this map[key] item 
             const std::string& paramExistsTag = "true" // value inserted for parameters with zero args 
            )
                : longPrefix_( longPrefix ), shortPrefix_( shortPrefix ),
                  reportUnknown_( reportUnknownParameters ), freeArgKey_( freeArgKey ),
                  paramExistsTag_( paramExistsTag )
    {
        Add( helpLongName, helpLongName, helpShortName, std::make_pair( 0, 0 ), true );
    }

    /// The "free argument key" is the key in the <name,values> argument map where arguments
    /// not associated with any parameter are stored.
    /// \return map key of "free" arguments 
    const std::string& GetFreeArgKey() const { return freeArgKey_; } 

    /// Add command line parameter.
    /// \param description description of command
    /// \param longName long name of command
    /// \param shortName short name of command
    /// \param numArguments min,max number of arguments
    /// \param optional true if parameter is optional, false otherwise
    void Add( const std::string& description,
              const std::string& longName,
              const std::string& shortName,
              std::pair< size_t, size_t > numArguments,
              bool optional )
    {
        if( entries_.find( longName ) != entries_.end() )
        {
            throw std::logic_error( "Error: entry " + longName + " already present" );
        }
        if( synonyms_.find( shortName ) != synonyms_.end() )
        {
            throw std::logic_error( "Error: entry " + shortName + " already present" );
        }
        entries_.insert( std::make_pair( 
                            longName, CommandEntry( description, longName, shortName, numArguments, optional ) ) );
        synonyms_.insert( std::make_pair( shortName, longName ) );
    }
    /// Parse command line; throw exception in case parsing fails. Information passed
    /// to command parameter entries is used to perform checks on the parsed parameters.
    /// \param numArgs number of strings to parse \attention in case 'reportUnknownParameters'
    ///        was set to 'true' the parsing will fail if the first argument is the program name
    ///        as is the case when passing the argc, argv paramters from main() directly; in this
    ///        case pass \c --argv and \c ++argc
    /// \param args array of C strings containing the command line arguments. Make sure to pass
    ///        \c --argc and ++argv in case \c reportUnknownParameters is set to \c true.
    ParsedEntries ParseCommandLine( int numArgs, char** args ) const
    {
        //put references to mandatory arguments in set
        std::set< std::string > mandatoryCommands;
        for( Entries::const_iterator it = entries_.begin(); it != entries_.end(); ++it )
        {
            if( it->second.optional == false ) mandatoryCommands.insert( it->first );
        }
        ParsedEntries parsedEntries;
        int i = 0;
        while( i < numArgs )
        {
            const std::string arg( args[ i ] );
            if( IsLongParameter( arg ) )
            {
                ++i;
                const std::string param( arg, longPrefix_.size() );
                Entries::const_iterator ei = entries_.find( param );
                if( ei == entries_.end() ) {
                    if( reportUnknown_ ) throw std::logic_error( "Error - unknown parameter " + arg );
                    else continue;
                }
                if( ei->second.optional == false )
                {
                    mandatoryCommands.erase( ei->first );
                }
                CommandParameters cp;
                i = AddParameterArguments( cp, numArgs, args, i );
                if( cp.size() < ei->second.numArguments.first || cp.size() > ei->second.numArguments.second )
                {
                    throw std::range_error( "Error: parameter " + ei->first + " wrong number of arguments" );
                }
                parsedEntries[ ei->second.longName ] = cp;
            }
            else if( IsShortParameter( arg ) )
            {
                ++i;
                Synonyms::const_iterator si = synonyms_.find( std::string( arg, shortPrefix_.size() ) );
                if( si == synonyms_.end() ) {
                    if( reportUnknown_ ) throw std::logic_error( "Error - unknown parameter " + arg );
                    else continue;
                }
                Entries::const_iterator ei = entries_.find( si->second );
                if( ei == entries_.end() ) {
                    if( reportUnknown_ ) throw std::logic_error( "Error: cannot find synonym of short command " + si->first );
                    else continue;
                }
                if( ei->second.optional == false )
                {
                    mandatoryCommands.erase( ei->first );
                }
                CommandParameters cp;
                i = AddParameterArguments( cp, numArgs, args, i );
                if( cp.size() < ei->second.numArguments.first || cp.size() > ei->second.numArguments.second )
                {
                    throw std::range_error( "Error: parameter " + ei->first + " wrong number of arguments" );
                }
                parsedEntries[ ei->second.longName ] = cp;
            }
            else
            {
                //not a parameter, add it to the list of free (not tied to a parameter) command line arguments 
                parsedEntries[ freeArgKey_ ].push_back( arg );
                ++i;
            }
        }
        if( !mandatoryCommands.empty() )
        {
            std::string msg( "Error - the following missing command line parameters are mandatory: " );
            for( std::set< std::string >::const_iterator i = mandatoryCommands.begin();
                 i != mandatoryCommands.end(); ++i )
            {
                msg += *i;
                msg += '\n';
            }
            throw std::logic_error( msg );
        }
        return parsedEntries;
    }
    /// Returns help text ready to be printed to standard output.
    /// \return help text built from command names and descriptions.
    std::string HelpText() const
    {
        std::ostringstream oss;
        for( Entries::const_iterator it = entries_.begin(); it != entries_.end(); ++it )
        {
            const CommandEntry& ci = it->second;
            oss << '\n' << longPrefix_ << ci.longName << ' ' << shortPrefix_ << ci.shortName << '\n';
            oss << ci.description << '\n';
            if( ci.optional == false ) oss << "Mandatory";
            else oss << "Optional";
            oss << '\n';
        }
        return oss.str();
    }
private:
    int AddParameterArguments( CommandParameters& cp, int numArgs, char** args, int i ) const
    {
        if( numArgs == 0 ) {
            cp.push_back( paramExistsTag_ );
        } else while( i < numArgs ) {
            if( IsParameter( args[ i ] ) ) break;
            cp.push_back( args[ i ] );
            ++i;
        }
        return i;
    }
    bool IsLongParameter( const std::string& p ) const
    {
        return p.find( longPrefix_ ) == 0;
    }
    bool IsShortParameter( const std::string& p ) const
    {
        return p.find( shortPrefix_ ) == 0;
    }
    bool IsParameter( const std::string& p ) const
    {
        return IsLongParameter( p ) || IsShortParameter( p );
    }
private:
    const std::string longPrefix_;
    const std::string shortPrefix_;
    Entries entries_;
    Synonyms synonyms_;
    bool reportUnknown_;
    std::string freeArgKey_;
    std::string paramExistsTag_;
private:
    CmdLine( const CmdLine& );
    CmdLine operator=( const CmdLine& );
};

inline bool Parsed( const std::string& key, const CmdLine::ParsedEntries& pe )
{
    return pe.find( key ) != pe.end();
}

template < class T >
inline T Get( const std::string& s )
{
    return T( s );
}

template <>
inline int Get< int >( const std::string& s )
{
    if( s.size() == 0 ) throw std::logic_error( "Error - Get<int>() - empty string" );
    return ::atoi( s.c_str() ); 
}

template <>
inline long Get< long >( const std::string& s )
{
    if( s.size() == 0 ) throw std::logic_error( "Error - Get<long>() - empty string" );
    return ::atol( s.c_str() ); 
}

template <>
inline double Get< double >( const std::string& s )
{
    if( s.size() == 0 ) throw std::logic_error( "Error - Get<double>() - empty string" );
    return ::atof( s.c_str() ); 
}

template <>
inline bool Get< bool >( const std::string& s )
{
    if( s.size() == 0 ) throw std::logic_error( "Error - Get<bool>() - empty string" );
    if( s == "true" || s == "True" || s == "TRUE" || s == "1" || s == "t" ) return true;
    if( s == "false" || s == "False" || s == "FALSE" || s == "0" || s == "f" ) return false;
    throw std::runtime_error( "Error - Get<bool>() - unknown boolean value: " + s );
    return false;
}

#endif //CMDLINE_H_