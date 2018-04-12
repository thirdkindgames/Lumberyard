/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include <AzFramework/Input/Buses/Requests/InputSystemCursorRequestBus.h>
#include <AzFramework/Input/Devices/InputDevice.h>
#include <AzFramework/Input/Channels/InputChannelDeltaWithSharedPosition2D.h>
#include <AzFramework/Input/Channels/InputChannelDigitalWithSharedPosition2D.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace AzFramework
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //! Defines a generic mouse input device, including the ids of all its associated input channels.
    //! Platform specific implementations are defined as private implementations so that creating an
    //! instance of this generic class will work correctly on any platform that supports mouse input,
    //! while providing access to the device name and associated channel ids on any platform through
    //! the 'null' implementation (primarily so that the editor can use them to setup input mappings).
    class InputDeviceMouse : public InputDevice,
                             public InputSystemCursorRequestBus::Handler
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////
        //! The id used to identify the primary mouse input device
        static const InputDeviceId Id;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Check whether an input device id identifies a mouse (regardless of index)
        //! \param[in] inputDeviceId The input device id to check
        //! \return True if the input device id identifies a mouse, false otherwise
        static bool IsMouseDevice(const InputDeviceId& inputDeviceId);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! All the input channel ids that identify standard mouse buttons. Though some mice support
        //! more than 5 buttons, it would be strange for a game to explicitly map them as this would
        //! exclude the majority of players who use a regular 3-button mouse. Developers should most
        //! likely expect players to assign additional mouse buttons to keyboard keys using software.
        //!
        //! Additionally, macOSX only supports three mouse buttons (left, right, and middle), so any
        //! cross-platform game should entirely ignore the 'Other1' and 'Other2' buttons, which have
        //! been implemented for windows simply to provide for backwards compatibility with CryInput.
        struct Button
        {
            static const InputChannelId Left;   //!< The left mouse button
            static const InputChannelId Right;  //!< The right mouse button
            static const InputChannelId Middle; //!< The middle mouse button
            static const InputChannelId Other1; //!< DEPRECATED: the x1 mouse button
            static const InputChannelId Other2; //!< DEPRECATED: the x2 mouse button

            //!< All mouse button ids
            static const AZStd::array<InputChannelId, 5> All;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! All the input channel ids that identify mouse movement. These input channels represent
        //! raw mouse movement before any system cursor ballistics have been applied, and so don't
        //! directly correlate to the mouse position (which is queried directly from the system).
        struct Movement
        {
            static const InputChannelId X; //!< Raw horizontal mouse movement over the last frame
            static const InputChannelId Y; //!< Raw vertical mouse movement over the last frame
            static const InputChannelId Z; //!< Raw mouse wheel movement over the last frame

            //!< All mouse movement ids
            static const AZStd::array<InputChannelId, 3> All;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Input channel id of the system cursor position normalized relative to the active window.
        //! The position obtained has had os ballistics applied, and is valid regardless of whether
        //! the system cursor is hidden or visible. When the system cursor has been constrained to
        //! the active window values will be in the [0.0, 1.0] range, but not when unconstrained.
        //! See also InputSystemCursorRequests::SetSystemCursorState and GetSystemCursorState.
        static const InputChannelId SystemCursorPosition;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Allocator
        AZ_CLASS_ALLOCATOR(InputDeviceMouse, AZ::SystemAllocator, 0);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Type Info
        AZ_RTTI(InputDeviceMouse, "{A509CA9D-BEAA-4124-9AAD-7381E46EBDD4}", InputDevice);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Reflection
        static void Reflect(AZ::ReflectContext* context);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Constructor
        explicit InputDeviceMouse();

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Disable copying (protected to workaround a VS2013 bug in std::is_copy_constructible)
        // https://connect.microsoft.com/VisualStudio/feedback/details/800328/std-is-copy-constructible-is-broken
    protected:
        AZ_DISABLE_COPY_MOVE(InputDeviceMouse);
    public:

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Destructor
        ~InputDeviceMouse() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AzFramework::InputDevice::GetInputChannelsById
        const InputChannelByIdMap& GetInputChannelsById() const override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AzFramework::InputDevice::IsSupported
        bool IsSupported() const override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AzFramework::InputDevice::IsConnected
        bool IsConnected() const override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AzFramework::InputDeviceRequests::TickInputDevice
        void TickInputDevice() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AzFramework::InputSystemCursorRequests::SetSystemCursorState
        void SetSystemCursorState(SystemCursorState systemCursorState) override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AzFramework::InputSystemCursorRequests::GetSystemCursorState
        SystemCursorState GetSystemCursorState() const override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AzFramework::InputSystemCursorRequests::SetSystemCursorPositionNormalized
        void SetSystemCursorPositionNormalized(AZ::Vector2 positionNormalized) override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AzFramework::InputSystemCursorRequests::GetSystemCursorPositionNormalized
        AZ::Vector2 GetSystemCursorPositionNormalized() const override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AzFramework::InputSystemCursorRequests::SetAllowCursorConstraint
        virtual void SetAllowCursorConstraint(bool constraintAllowed) override;

    protected:
        ////////////////////////////////////////////////////////////////////////////////////////////
        ///@{
        //! Alias for verbose container class
        using ButtonChannelByIdMap = AZStd::unordered_map<InputChannelId,
                                                          InputChannelDigitalWithSharedPosition2D*>;
        using MovementChannelByIdMap = AZStd::unordered_map<InputChannelId,
                                                            InputChannelDeltaWithSharedPosition2D*>;
        ///@}

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Variables
        InputChannelByIdMap                    m_allChannelsById;       //!< All mouse channels
        ButtonChannelByIdMap                   m_buttonChannelsById;    //!< Mouse button channels
        MovementChannelByIdMap                 m_movementChannelsById;  //!< Mouse movement channels
        InputChannelDeltaWithSharedPosition2D* m_cursorPositionChannel; //!< Cursor position channel
        InputChannel::SharedPositionData2D     m_cursorPositionData2D;  //!< Shared cursor position

    public:
        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Base class for platform specific implementations of mouse input devices
        class Implementation
        {
        public:
            ////////////////////////////////////////////////////////////////////////////////////////
            // Allocator
            AZ_CLASS_ALLOCATOR(Implementation, AZ::SystemAllocator, 0);

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Default factory create function
            //! \param[in] inputDevice Reference to the input device being implemented
            static Implementation* Create(InputDeviceMouse& inputDevice);

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Constructor
            //! \param[in] inputDevice Reference to the input device being implemented
            Implementation(InputDeviceMouse& inputDevice);

            ////////////////////////////////////////////////////////////////////////////////////////
            // Disable copying
            AZ_DISABLE_COPY_MOVE(Implementation);

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Destructor
            virtual ~Implementation();

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Query the connected state of the input device
            //! \return True if the input device is currently connected, false otherwise
            virtual bool IsConnected() const = 0;

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Attempt to set the state of the system cursor
            //! \param[in] systemCursorState The desired system cursor state
            virtual void SetSystemCursorState(SystemCursorState systemCursorState) = 0;

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Get the current state of the system cursor
            //! \return The current state of the system cursor
            virtual SystemCursorState GetSystemCursorState() const = 0;

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Attempt to set the system cursor position normalized relative to the active window
            //! \param[in] positionNormalized The desired system cursor position normalized
            virtual void SetSystemCursorPositionNormalized(AZ::Vector2 positionNormalized) = 0;

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Get the current system cursor position normalized relative to the active window. The
            //! position obtained has had os ballistics applied, and is valid regardless of whether
            //! the system cursor is hidden or visible. When the cursor has been constrained to the
            //! active window the values will be in the [0.0, 1.0] range, but not when unconstrained.
            //! See also InputSystemCursorRequests::SetSystemCursorState and GetSystemCursorState.
            //! \return The current system cursor position normalized relative to the active window
            virtual AZ::Vector2 GetSystemCursorPositionNormalized() const = 0;

            ////////////////////////////////////////////////////////////////////////////////////////
            // Allow global enabling/disabling of mouse cursor capture.
            virtual void SetAllowCursorConstraint(bool) {};

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Tick/update the input device to broadcast all input events since the last frame
            virtual void TickInputDevice() = 0;

        protected:
            ////////////////////////////////////////////////////////////////////////////////////////
            //! Queue raw button events to be processed in the next call to ProcessRawEventQueues.
            //! This function is not thread safe and so should only be called from the main thread.
            //! \param[in] inputChannelId The input channel id
            //! \param[in] rawButtonState The raw button state
            void QueueRawButtonEvent(const InputChannelId& inputChannelId, bool rawButtonState);

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Queue raw movement events to be processed in the next call to ProcessRawEventQueues.
            //! This function is not thread safe and so should only be called from the main thread.
            //! \param[in] inputChannelId The input channel id
            //! \param[in] rawMovementDelta The raw movement delta
            void QueueRawMovementEvent(const InputChannelId& inputChannelId, float rawMovementDelta);

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Process raw input events that have been queued since the last call to this function.
            //! This function is not thread safe, and so should only be called from the main thread.
            void ProcessRawEventQueues();

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Reset the state of all this input device's associated input channels
            void ResetInputChannelStates();

            ////////////////////////////////////////////////////////////////////////////////////////
            //! Alias for verbose container class
            ///@{
            using RawButtonEventQueueByIdMap = AZStd::unordered_map<InputChannelId, AZStd::vector<bool>>;
            using RawMovementEventQueueByIdMap = AZStd::unordered_map<InputChannelId, AZStd::vector<float>>;
            ///@}

        private:
            ////////////////////////////////////////////////////////////////////////////////////////
            // Variables
            InputDeviceMouse&            m_inputDevice;                //!< Reference to the device
            RawButtonEventQueueByIdMap   m_rawButtonEventQueuesById;   //!< Raw button events by id
            RawMovementEventQueueByIdMap m_rawMovementEventQueuesById; //!< Raw movement events by id
        };

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Set the implementation of this input device
        //! \param[in] implementation The new implementation
        void SetImplementation(AZStd::unique_ptr<Implementation> impl) { m_pimpl = AZStd::move(impl); }

    private:
        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Private pointer to the platform specific implementation
        AZStd::unique_ptr<Implementation> m_pimpl;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Helper class that handles requests to create a custom implementation for this device
        InputDeviceImplementationRequestHandler<InputDeviceMouse> m_implementationRequestHandler;
    };
} // namespace AzFramework
