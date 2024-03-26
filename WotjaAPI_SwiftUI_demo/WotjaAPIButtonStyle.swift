//
// BlueButton.swift
// WotjaAPI_SwiftUI_demo
//
// Copyright (C) 2007-2024 Intermorphic Limited. All rights reserved.
// Licensed under the MIT License.
// Note: For the avoidance of doubt use of Wotja is governed by the Wotja EULA to be found at https://wotja.com/legal/eula
//

import SwiftUI

struct WotjaAPIButtonStyle: ButtonStyle {
    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .padding(8)
            .background(Color(red: 0, green: 0, blue: 0.5))
            .foregroundStyle(.white)
            .clipShape(RoundedRectangle(cornerRadius: 4))
            .minimumScaleFactor(0.01)
            .frame(minWidth: 64.0, minHeight:60.0)
            .multilineTextAlignment(.center)
            .scaleEffect(configuration.isPressed ? 0.9 : 1)
    }
}
