#[cfg(test)]
mod tests {
    use iroha_version::{
        error::{Error, Result},
        json::*,
        RawVersioned, Version,
    };
    use iroha_version_derive::{declare_versioned, version};
    use parity_scale_codec::{Decode, Encode};
    use serde::{Deserialize, Serialize};

    mod model_1 {
        use super::*;

        declare_versioned!(VersionedMessage 1..3);

        #[version(n = 1, versioned = "VersionedMessage")]
        #[derive(Debug, Clone, Decode, Encode, Serialize, Deserialize)]
        pub struct Message;

        #[version(n = 2, versioned = "VersionedMessage")]
        #[derive(Debug, Clone, Decode, Encode, Serialize, Deserialize)]
        pub struct Message2;
    }

    mod model_2 {
        use super::*;

        declare_versioned!(VersionedMessage 1..4);

        #[version(n = 1, versioned = "VersionedMessage")]
        #[derive(Debug, Clone, Decode, Encode, Serialize, Deserialize)]
        pub struct Message;

        #[version(n = 2, versioned = "VersionedMessage")]
        #[derive(Debug, Clone, Decode, Encode, Serialize, Deserialize)]
        pub struct Message2;

        #[version(n = 3, versioned = "VersionedMessage")]
        #[derive(Debug, Clone, Decode, Encode, Serialize, Deserialize)]
        pub struct Message3(pub String);
    }

    #[test]
    fn supported_version() -> Result<()> {
        use model_1::*;

        let versioned_message: VersionedMessage = Message.into();
        let json = versioned_message.to_versioned_json_str()?;
        let decoded_message = VersionedMessage::from_versioned_json_str(&json)?;
        match decoded_message {
            VersionedMessage::V1(message) => {
                let _message: Message = message.into();
                Ok(())
            }
            VersionedMessage::V2(message) => {
                let _message: Message2 = message.into();
                Err(Error::msg("Should have been message v1."))
            }
            _ => Err(Error::msg("Unsupported version.")),
        }
    }

    #[test]
    fn unsupported_version() -> Result<()> {
        let json = {
            use model_2::*;

            let versioned_message: VersionedMessage = Message3("test string".to_string()).into();
            versioned_message.to_versioned_json_str()?
        };

        use model_1::*;

        let raw_string = "{\"version\":\"3\",\"content\":\"test string\"}";
        let decoded_message = VersionedMessage::from_versioned_json_str(&json)?;
        assert!(!decoded_message.is_supported());
        match decoded_message {
            VersionedMessage::UnsupportedVersion(unsupported_version) => {
                assert_eq!(unsupported_version.version, 3);
                if let RawVersioned::Json(json) = unsupported_version.raw {
                    assert_eq!(json, raw_string);
                    Ok(())
                } else {
                    Err(Error::msg("Should be json."))
                }
            }
            _ => Err(Error::msg("Should be an unsupported version")),
        }
    }
}
