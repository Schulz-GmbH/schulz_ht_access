import { defineComponent, computed } from 'vue';
import { useI18n } from 'vue-i18n';

// Components

export default defineComponent({
	name: 'AboutUs',
	components: {},
	setup() {
		const { t } = useI18n();

		// HTML-Link für Simon
		const simonLink = `<a href="https://github.com/SimonMarcelLinden" target="_blank" rel="noopener">Simon Marcel Linden</a>`;

		// Computed für Team (zwei Zeilen: Admin + Full-Stack)
		const fullTeamHtml = computed(() => {
			const adminText = t('pages.about-us.team.itAdmin', { link: simonLink });
			const devText = t('pages.about-us.team.fullStack', { link: simonLink });
			return `${adminText}<br/>${devText}`;
		});

		// Computed für Support-Abschnitt
		const issuesLink = `<a href="https://github.com/Schulz-GmbH/schulz_ht_access/issues" target="_blank" rel="noopener">GitHub Issues</a>`;
		const mailLink = `<a href="mailto:entwicklung@schulz-hygiene.de">entwicklung@schulz-hygiene.de</a>`;

		const supportHtml = computed(() =>
			t('pages.about-us.support', {
				issuesLink,
				email: mailLink,
			})
		);

		return {
			fullTeamHtml,
			supportHtml,
		};
	},
});
