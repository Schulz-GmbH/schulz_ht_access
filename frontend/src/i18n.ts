import { createI18n } from 'vue-i18n';
import en from '@/locales/en.json';
import de from '@/locales/de.json';

export const messages = { en, de };

export type AppLocale = 'de' | 'en';
export type Locale = keyof typeof messages;

export const i18n = createI18n({
	legacy: false,
	globalInjection: true,
	locale: 'de' as AppLocale,
	fallbackLocale: 'en' as AppLocale,
	messages,
});

declare module 'vue-i18n' {
	export interface DefineLocaleMessageSchema {
		greeting: string;
		message: { welcome: string };
	}
}
