<template>
	<div class="license p-6 max-w-3xl mx-auto">
		<h1 class="text-3xl font-bold mb-4">{{ $t('pages.license.title') }}</h1>
		<p class="mb-4">
			{{ $t('pages.license.intro') }}
		</p>
		<section class="conditions mb-6">
			<h2 class="text-2xl font-semibold mb-2">{{ $t('pages.license.sections.allowed.heading') }}</h2>
			<ul class="list-disc list-inside">
				<li>{{ $t('pages.license.sections.allowed.items.installation') }}</li>
				<li>{{ $t('pages.license.sections.allowed.items.updates') }}</li>
			</ul>
		</section>
		<section class="prohibited">
			<h2 class="text-2xl font-semibold mb-2">Untersagte Handlungen</h2>
			<ul class="list-disc list-inside">
				<li>{{ $t('pages.license.sections.prohibited.items.transfer') }}</li>
				<li>{{ $t('pages.license.sections.prohibited.items.copyModify') }}</li>
				<li>{{ $t('pages.license.sections.prohibited.items.reverse') }}</li>
			</ul>
		</section>
		<p class="mt-6 text-sm text-gray-600">
			{{ $t('pages.license.footer') }}
		</p>
	</div>
</template>

<script lang="ts">
export { default } from './license'

</script>

<style src="./license.scss" scoped></style>
