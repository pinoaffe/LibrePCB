/*
 * EDA4U - Professional EDA for everyone!
 * Copyright (C) 2013 Urban Bruhin
 * http://eda4u.ubruhin.ch/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PROJECT_IF_ERCMSGPROVIDER_H
#define PROJECT_IF_ERCMSGPROVIDER_H

/*****************************************************************************************
 *  Forward Declarations
 ****************************************************************************************/

namespace project {
class ErcMsg; // all classes which implement IF_ErcMsgProvider will need this declaration
}

/*****************************************************************************************
 *  Macros
 ****************************************************************************************/

/**
 * @note    The specified class name should be unique only in the namespace #project,
 *          so we won't use the namespace as a prefix. Simple use the class name.
 *
 * @warning Do not change the name of an existing class if you don't know what you're doing!
 */
#define DECLARE_ERC_MSG_CLASS_NAME(msgOwnerClassName) \
public: \
    virtual const char* getErcMsgOwnerClassName() const noexcept {return #msgOwnerClassName;} \
private:

/*****************************************************************************************
 *  Class IF_ErcMsgProvider
 ****************************************************************************************/

namespace project {

/**
 * @brief The IF_ErcMsgProvider class
 *
 * @author ubruhin
 * @date 2015-02-02
 */
class IF_ErcMsgProvider
{
    public:

        // Constructors / Destructor
        IF_ErcMsgProvider() {}
        virtual ~IF_ErcMsgProvider() {}

        // Getters
        virtual const char* getErcMsgOwnerClassName() const noexcept = 0;
};

} // namespace project

#endif // PROJECT_IF_ERCMSGPROVIDER_H