<template>
	<div class="layout-logo-container" :class="{ 'collapse': props.collapse, 'layout-mode-top': isTop }">
		<transition name="layout-logo-fade">
			<router-link v-if="props.collapse" key="collapse" to="/">
				<img :src="logo" class="layout-logo" :alt="$t('components.logo.alt')"
					:title="$t('components.logo.title')" />
			</router-link>
			<router-link v-else key="expand" to="/">
				<img :src="logoText1" class="layout-logo-text" :alt="$t('components.logo.alt')"
					:title="$t('components.logo.title')" />
			</router-link>
		</transition>
	</div>
</template>

<script lang="ts">

export { default } from './logo'

</script>

<style scoped src="./logo.scss"></style>
