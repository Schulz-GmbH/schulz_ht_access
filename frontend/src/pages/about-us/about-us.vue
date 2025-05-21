<!-- src/views/AboutUs.vue -->
<template>
	<div class="about-us p-6 max-w-3xl mx-auto">
		<h1 class="text-3xl font-bold mb-4">{{ $t('pages.about-us.title') }}</h1>
		<p class="mb-4">
			{{ $t('pages.about-us.intro') }}
		</p>
		<section class="versions mb-6">
			<h2 class="text-2xl font-semibold mb-2">{{ $t('pages.about-us.versionHeading') }}</h2>
			<ul class="list-disc list-inside">
				<li>
					<strong>v1.0.0</strong> {{ $t('pages.about-us.versions.release') }}
				</li>
			</ul>
		</section>
		<section class="team">
			<h2 class="text-2xl font-semibold mb-2">{{ $t('pages.about-us.teamHeading') }}</h2>
			<p v-html="fullTeamHtml"></p>
		</section>
		<section class="support mb-6">
			<h2 class="text-2xl font-semibold mb-2">{{ $t('pages.about-us.supportHeading') }}</h2>
			<p v-html="supportHtml"></p>
		</section>
	</div>
</template>

<script lang="ts">
export { default } from './about-us'

</script>

<style src="./about-us.scss" scoped></style>
