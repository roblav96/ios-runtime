#pragma once

namespace NativeScript {
    class LiveSyncSourceProvider : public JSC::SourceProvider {
    public:
        static Ref<LiveSyncSourceProvider> create(const WTF::String& source, const WTF::String& url, const WTF::TextPosition& startPosition = WTF::TextPosition::minimumPosition()) {
            return adoptRef(*new LiveSyncSourceProvider(source, url, startPosition));
        }
        
        const String& source() const override {
            return this->m_source;
        }
        
        void setSource(WTF::String source) {
            bool shit = this->m_source == source;
            UNUSED_PARAM(shit);
            this->m_source = source;
        }
        
    private:
        LiveSyncSourceProvider(const WTF::String& source, const WTF::String& url, const WTF::TextPosition& startPosition)
            : JSC::SourceProvider(url, startPosition)
            , m_source(source) {
            
        }
        
        WTF::String m_source;
    };
}

