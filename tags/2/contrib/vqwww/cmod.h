#ifndef __CMOD_H
#define __CMOD_H

//! Base class for all modules.
class cmod {
        protected:
                cmod() {}
        public:
                //! codes returned by run()
                enum {
                        loopa, /*!< do module check again, but don't end this turn */
                        loopaf, /*!< loop again, after finishing this */
                        notme, /*!< i did nothing */
                        done,    /*!< this module was selected, action was taken, you can exit */
                        sessclose /*!< close session (for example user wants to log out)*/
                };
               
                //! flags used by modules to report their capatibilities
                enum _modinfo {
                        mi_none = 0, /*!< i do nothing */
                        mi_conf = 1, /*!< i'm configurable, cconfmod will use it */
                        mi_menu = 2 /*! put me in menu */
                };
                
                /// information about module's capatibilities
                unsigned modinfo;

                virtual const char *id()=0; ///< return module's id, usually it shoud be class name without prefix 'c', and postfix "mod"
                virtual const char *name()=0;
                virtual const char *menu()=0; ///< xml code of menu's entry
                virtual const char *conf()=0; ///< xml code of configuration's entry
                virtual char run()=0; ///< perform module's action
};

#endif
